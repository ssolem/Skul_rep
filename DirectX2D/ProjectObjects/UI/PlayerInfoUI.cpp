#include "Framework.h"
#include "PlayerInfoUI.h"
#include "PlayerInfoSlot.h"
#include "Objects/UI/MultiPanelProgressBar.h"
#include "ProjectObjects/Character/Player.h"
#include "ProjectObjects/Character/SkulClass.h"
#include "ProjectObjects/Character/Skill.h"
#include "ProjectObjects/Stage.h"

static const Vector2 MAIN_PORTRAIT_SIZE(45.0f, 45.0f);
static const Vector2 SUB_PORTRAIT_SIZE(21.0f, 21.0f);
static const Vector2 SKILL_ICON_SIZE(24.0f, 24.0f);
static const Vector2 SUB_SKILL_ICON_SIZE(18.0f, 18.0f);
static const Vector2 ESSENCE_ICON_SIZE(26.0f, 26.0f);

PlayerInfoUI::PlayerInfoUI(Player* player)
	:player(player)
{
	SetTag("PlayerInfoUI");
	wstring path = L"ProjectTextures/UI/Player_Frame/";

	MultiPanelProgressBar::Textures textures;
	textures.base = L"ProjectTextures/UI/HealthBar_Enemy/EnemyHealthBar_Base.png";
	textures.health = path + L"HealthBar/Player_HealthBar.png";
	textures.heal = path + L"HealthBar/Player_HealthBar_Heal.png";
	textures.shield = path + L"HealthBar/Player_HealthBar_Shield.png";
	textures.damage = path + L"HealthBar/Player_HealthBar_Damage.png";
	gauge = new MultiPanelProgressBar(textures, Vector2(118.0f, 10.0f));
	gauge->SetParent(this);
	gauge->SetTag("PlayerInfo_HPGauge");

	gauge->Init((float)player->GetMaxHP(), (float)player->GetCurHP());
	gauge->Pos() += Vector2(18.0f, -16.0f);

	mainFrame = new Quad(path + L"Player_Normal_Frame.png");
	mainFrame->SetParent(this);
	mainFrame->SetTag("PlayerInfo_MainFrame");

	mainPortrait = new Quad(MAIN_PORTRAIT_SIZE);
	mainPortrait->SetParent(this);
	mainPortrait->SetTag("PlayerInfo_MainPortrait");
	mainPortrait->Pos() += Vector2(-55.5f, 9.5f);

	firstSkillIcon = new Quad(SKILL_ICON_SIZE);
	firstSkillIcon->SetParent(this);
	firstSkillIcon->SetTag("PlayerInfo_Skill1");
	firstSkillIcon->Pos() += Vector2(-14.0f, 6.0f);

	secondSkillIcon = new PlayerInfoSlot(path + L"Player_Skill2_Frame.png", Vector2(0.5f, 1.0f), SKILL_ICON_SIZE);
	secondSkillIcon->SetParent(this);
	secondSkillIcon->SetTag("PlayerInfo_Skill2");
	secondSkillIcon->Pos() += Vector2(15.5f, 5.0f);

	subPortrait = new PlayerInfoSlot(path + L"Player_Subskull_Frame.png", Vector2(0.0f, 0.0f), SUB_PORTRAIT_SIZE);
	subPortrait->SetParent(this);
	subPortrait->SetTag("PlayerInfo_SubPortrait");
	subPortrait->Pos() += Vector2(-65.0f, -13.0f);

	firstSubSkillIcon = new PlayerInfoSlot(path + L"Player_Subskill1_Frame.png", Vector2(0.0f, 0.0f), SUB_SKILL_ICON_SIZE);
	firstSubSkillIcon->SetParent(this);
	firstSubSkillIcon->SetTag("PlayerInfo_SubSkill1");
	firstSubSkillIcon->Pos() += Vector2(46.0f, 1.0f);
	firstSubSkillIcon->SetDim(true);

	secondSubSkillIcon = new PlayerInfoSlot(path + L"Player_Subskill2_Frame.png", Vector2(0.0f, 0.0f), SUB_SKILL_ICON_SIZE);
	secondSubSkillIcon->SetParent(this);
	secondSubSkillIcon->SetTag("PlayerInfo_SubSkill2");
	secondSubSkillIcon->Pos() += Vector2(66.0f, 1.0f);
	secondSubSkillIcon->SetDim(true);

	essenceIcon = new PlayerInfoSlot(path + L"Player_Quintessence_Frame.png", Vector2(0.0f, 0.0f), ESSENCE_ICON_SIZE);
	essenceIcon->SetParent(this);
	essenceIcon->SetTag("PlayerInfo_Essence");
	essenceIcon->Pos() += Vector2(91.5f, -14.0f);

	mainPortrait->SetPixelShader(L"CoolDown.hlsl");
	firstSkillIcon->SetPixelShader(L"CoolDown.hlsl");
	portraitCoolBuffer = new FloatValueBuffer();
	portraitCoolBuffer->Get()[0] = 1.0f;
	skill1CoolBuffer = new FloatValueBuffer();
	skill1CoolBuffer->Get()[0] = 1.0f;

	Refresh();
	//정수는 구현 후

	CreateCounters();
}

PlayerInfoUI::~PlayerInfoUI()
{
	delete portraitCoolBuffer;
	delete skill1CoolBuffer;
	delete gauge;
	delete mainFrame;
	delete mainPortrait;
	delete firstSkillIcon;
	delete secondSkillIcon;
	delete subPortrait;
	delete firstSubSkillIcon;
	delete secondSubSkillIcon;
	delete essenceIcon;

	for (Quad* icon : counterIcons)
		delete icon;
	delete counterRoot;
}

void PlayerInfoUI::Update()
{
	UpdateWorld();
	gauge->Update();
	mainFrame->UpdateWorld();
	mainPortrait->UpdateWorld();
	firstSkillIcon->UpdateWorld();
	//슬롯은 Update()에서 자기 + frame/icon 월드를 함께 갱신
	secondSkillIcon->Update();
	subPortrait->Update();
	firstSubSkillIcon->Update();
	secondSubSkillIcon->Update();
	essenceIcon->Update();

	UpdateCounters();
}

void PlayerInfoUI::Render()
{
	if (!Active()) return;

	gauge->Render();                //프레임의 체력바 구멍 아래에 깔리도록 먼저 그림
	mainFrame->Render();
	//텍스처가 없는 Quad를 그리면 PSSet에서 크래시 - 비어 있으면 건너뜀
	if (mainPortrait->GetTexture())
	{
		portraitCoolBuffer->SetPS(1);
		mainPortrait->Render();
	}
	if (firstSkillIcon->GetTexture())
	{
		skill1CoolBuffer->SetPS(1);
		firstSkillIcon->Render();
	}

	//슬롯은 아이콘이 없으면 스스로 Render를 건너뜀
	secondSkillIcon->Render(showAllSlots);
	subPortrait->Render(showAllSlots);
	firstSubSkillIcon->Render(showAllSlots);
	secondSubSkillIcon->Render(showAllSlots);
	essenceIcon->Render(showAllSlots);

	RenderCounters();
	UpdateCoolDowns();
}

void PlayerInfoUI::RenderUI()
{
	if (!ImGui::TreeNode("PlayerInfoUI")) return;

	ImGui::Checkbox("Show All Slots", &showAllSlots);

	Transform::RenderUI();	//HUD 전체 위치/배율
	gauge->RenderUI();
	mainFrame->RenderUI();
	mainPortrait->RenderUI();
	firstSkillIcon->RenderUI();
	secondSkillIcon->RenderUI();
	subPortrait->RenderUI();
	firstSubSkillIcon->RenderUI();
	secondSubSkillIcon->RenderUI();
	essenceIcon->RenderUI();

	counterRoot->RenderUI();
	ImGui::DragFloat("Counter Row Gap", &counterRowGap, 0.5f);
	ImGui::DragFloat2("Counter Text Offset", (float*)&counterTextOffset, 0.5f);

	ImGui::TreePop();
}

//메인,서브 바뀔때. 시작할떄
void PlayerInfoUI::Refresh()
{
	SkulClass* mainSkul = player->GetCurSkul();
	mainPortrait->SetTexture(mainSkul->GetMainPortrait());
	firstSkillIcon->SetTexture(GetSkillIcon(mainSkul, 0));
	secondSkillIcon->SetIcon(GetSkillIcon(mainSkul, 1));

	SkulClass* subSkul = player->GetSubSkul();
	subPortrait->SetIcon(subSkul ? subSkul->GetSubPortrait() : nullptr);
	firstSubSkillIcon->SetIcon(GetSkillIcon(subSkul, 0));
	secondSubSkillIcon->SetIcon(GetSkillIcon(subSkul, 1));

	essenceIcon->SetIcon(nullptr);  //정수 구현 후 연결
}

void PlayerInfoUI::CreateCounters()
{
	counterRoot = new Transform();
	counterRoot->SetTag("PlayerInfo_Counters");
	counterRoot->Scale() = SKUL_DEFAULT_ASSET_SIZE;
	counterRoot->Pos() = Vector2(WIN_WIDTH - 110.0f, 30.0f);	//오른쪽 아래 - 맨 아래 줄 아이콘 중심

	wstring path = L"ProjectTextures/Common/UI/";
	const wstring files[COUNTER_MAX] = { L"Enemy_Icon.png", L"Gold_Icon.png", L"DarkQuartz_Icon.png", L"BoneChip_Icon.png" };

	for (int i = 0; i < COUNTER_MAX; i++)
	{
		counterIcons[i] = new Quad(path + files[i]);
		counterIcons[i]->SetParent(counterRoot);
	}
}

void PlayerInfoUI::UpdateCounters()
{
	Stage* stage = player->GetStage();
	counterValues[ENEMY] = stage ? stage->GetRemainEnemyCount() : 0;
	counterValues[GOLD] = player->GetGold();
	counterValues[DARK_QUARTZ] = player->GetDarkQuartz();
	counterValues[BONE_CHIP] = player->GetBoneChip();

	counterRoot->UpdateWorld();

	for (int i = 0; i < COUNTER_MAX; i++)
	{
		counterIcons[i]->Pos() = Vector2(0.0f, (COUNTER_MAX - 1 - i) * counterRowGap);
		counterIcons[i]->UpdateWorld();
	}
}

void PlayerInfoUI::RenderCounters()
{
	for (int i = 0; i < COUNTER_MAX; i++)
	{
		counterIcons[i]->Render();

		//RenderText는 pos를 사각형 중심으로 씀 -> 왼쪽 위 기준으로 바꿔서 넘김
		Vector2 leftTop = counterIcons[i]->GlobalPos() + Vector2(counterTextOffset.x, counterTextOffset.y);
		Float2 center = { leftTop.x + counterTextSize.x * 0.5f, leftTop.y - counterTextSize.y * 0.5f };

		Font::Get()->RenderText(to_string(counterValues[i]), center,
			{ counterTextSize.x, counterTextSize.y }, "Default", "White");
	}
}

Texture* PlayerInfoUI::GetSkillIcon(SkulClass* skul, int index)
{
	if (skul == nullptr) return nullptr;

	vector<Skill*>& skills = skul->GetCurSkill();
	if (index >= skills.size() || skills[index] == nullptr) return nullptr;

	return skills[index]->GetIcon();
}

void PlayerInfoUI::Init(float maxHP, float curHP)
{
	gauge->Init(maxHP, curHP);
}

void PlayerInfoUI::SetHP(float hp)
{
	gauge->SetHP(hp);
}

void PlayerInfoUI::SetMaxHP(float maxHP)
{
	gauge->SetMaxHP(maxHP);
}

void PlayerInfoUI::SetShield(float shield)
{
	gauge->SetShield(shield);
}

void PlayerInfoUI::UpdateCoolDowns()
{
	SkulClass* main = player->GetCurSkul();
	SkulClass* sub = player->GetSubSkul();

	portraitCoolBuffer->Get()[0] = 1.0f; 
	skill1CoolBuffer->Get()[0] = GetSkillCoolRatio(main, 0);
	secondSkillIcon->SetCoolDown(GetSkillCoolRatio(main, 1));
	firstSubSkillIcon->SetCoolDown(GetSkillCoolRatio(sub, 0));
	secondSubSkillIcon->SetCoolDown(GetSkillCoolRatio(sub, 1));
}

float PlayerInfoUI::GetSkillCoolRatio(SkulClass* skul, int index)
{
	if (skul == nullptr) return 1.0f;

	vector<Skill*>& skills = skul->GetCurSkill();
	if (index >= skills.size() || skills[index] == nullptr) return 1.0f;

	return skills[index]->GetCoolDownRatio();
}
