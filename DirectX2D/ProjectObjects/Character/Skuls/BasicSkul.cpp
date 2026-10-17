#include "Framework.h"
#include "BasicSkul.h"
#include "../Animator.h"
#include "../Character.h"
#include "ProjectObjects/Character/Skuls/Skills/BasicSkullSkill1.h"
#include "ProjectObjects/Character/Skuls/Skills/BasicSkullSkill2.h"
#include "ProjectObjects/Objects/SkulObject.h"
#include "ProjectObjects/Stage.h"
#include "../Stat.h"
#include "../Enemy/Normal/Enemy.h"


BasicSkul::BasicSkul()
{
	SetTag("BasicSkul");
}

BasicSkul::~BasicSkul()
{
	delete head;
}

void BasicSkul::Update()
{
	SkulClass::Update();

	if (!head->Active()) return;

	//1번 스킬로 날아가는 중(LAUNCHING)일 때만 충돌 검사
	if (head->GetFlightState() == FlightState::LAUNCHING)
	{
		//오브젝트: 날려 보내고 머리는 계속 날아감 -> 겹친 것 전부를 한 번씩 (중복은 스킬의 alreadyHit이 거름)
		for (SkulObject* object : owner->GetStage()->CheckAllOtherObjectCollision(head))
			throwSkill->OnHeadHitObject(object);

		//적: 맞으면 PHYSICS로 바뀌어 한 번만 타격
		if (Enemy* enemy = owner->GetStage()->CheckEnemyCollision(head->GetCollider()))
			throwSkill->OnHeadHit(enemy);
	}

	bool touching = owner->GetCollider()->IsCollision(head->GetCollider());

	//던진 머리가 캐릭터에서 한 번 떨어진 뒤부터 회수 가능
	if (!canRetrieveHead)
	{
		if (!touching) canRetrieveHead = true;
		return;
	}

	//2번 스킬을 쓰지 않아도 머리에 직접 닿으면 다시 주움
	if (touching)
		RetrieveHead();
}

void BasicSkul::RetrieveHead()
{
	SetTag("BasicSkul");
	head->SetActive(false);
	canRetrieveHead = false;

	//같은 이동 상태(걷는 중 등)로 주우면 PlayAnim이 클립을 안 바꾸므로 머리 있는 클립으로 강제 교체
	owner->RefreshMoveClip();
}

void BasicSkul::Render()
{
	SkulClass::Render();
}

void BasicSkul::Init(Character* owner, Animator* animator)
{
	this->owner = owner;

	CreateAnimations(animator);
	SetStats();
	SetImages();
	CreateHead();
	CreateSkills();

	curSkills[0] = skills[0];
	curSkills[1] = skills[1];

	initialized = true;
}

void BasicSkul::CreateAnimations(Animator* animator)
{
	// tag는 등록 키 접두어로만 넘김 (등록 키 = tag+fileName은 Animator 내부에서 조합), fileName은 실제 파일 탐색용

	SetTag("BasicSkul");
	//BasicSkul
	animator->CreateClipByAtlas(tag, "AttackA", L"ProjectTextures/Skul/Atlas", false);
	animator->CreateClipByAtlas(tag, "AttackB", L"ProjectTextures/Skul/Atlas", false);
	animator->CreateClipByAtlas(tag, "Dash", L"ProjectTextures/Skul/Atlas", false);

	animator->CreateClipByAtlas(tag, "Dead_Freeze", L"ProjectTextures/Skul/Atlas", false);
	animator->CreateClipByAtlas(tag, "Fall_Dead", L"ProjectTextures/Skul/Atlas", false);

	animator->CreateClipByAtlas(tag, "FallRepeat", L"ProjectTextures/Skul/Atlas");
	animator->CreateClipByAtlas(tag, "Fall", L"ProjectTextures/Skul/Atlas", false);
	animator->FindClip(tag + "Fall")->SetEvent([this, animator]() {animator->PlayClip(tag + "FallRepeat"); });

	animator->CreateClipByAtlas(tag, "Idle", L"ProjectTextures/Skul/Atlas");
	animator->CreateClipByAtlas(tag, "Jump", L"ProjectTextures/Skul/Atlas", false);
	animator->CreateClipByAtlas(tag, "JumpAttack", L"ProjectTextures/Skul/Atlas", false);

	animator->CreateClipByAtlas(tag, "Skill", L"ProjectTextures/Skul/Atlas", false);
	animator->CreateClipByAtlas(tag, "Skill_Reborn", L"ProjectTextures/Skul/Atlas", false);
	animator->CreateClipByAtlas(tag, "Switch", L"ProjectTextures/Skul/Atlas", false);
	animator->CreateClipByAtlas(tag, "Walk", L"ProjectTextures/Skul/Atlas");

	SetTag("BasicSkulHeadless");
	//HeadLess
	animator->CreateClipByAtlas(tag, "AttackA", L"ProjectTextures/Skul_Headless/Atlas", false);
	animator->CreateClipByAtlas(tag, "AttackB", L"ProjectTextures/Skul_Headless/Atlas", false);
	animator->CreateClipByAtlas(tag, "Dash", L"ProjectTextures/Skul_Headless/Atlas", false);

	animator->CreateClipByAtlas(tag, "FallRepeat", L"ProjectTextures/Skul_Headless/Atlas");
	animator->CreateClipByAtlas(tag, "Fall", L"ProjectTextures/Skul_Headless/Atlas", false);
	animator->FindClip(tag + "Fall")->SetEvent([this, animator]() {animator->PlayClip(tag + "FallRepeat"); });

	animator->CreateClipByAtlas(tag, "Idle", L"ProjectTextures/Skul_Headless/Atlas");
	animator->CreateClipByAtlas(tag, "Jump", L"ProjectTextures/Skul_Headless/Atlas", false);
	animator->CreateClipByAtlas(tag, "JumpAttack", L"ProjectTextures/Skul_Headless/Atlas", false);

	animator->CreateClipByAtlas(tag, "Walk", L"ProjectTextures/Skul_Headless/Atlas");

	SetTag("BasicSkul");
	SetAnimations();
	CreateComboAttacks(animator);
}

void BasicSkul::SetAnimations()
{
	//BASIC_ATTACK은 콤보 스텝별 클립 이름이 CreateComboAttacks()의 ComboAttackData 안에 들어있어서 여기 map엔 안 넣음
	actionClips[ActionType::JUMP_ATTACK] = { "JumpAttack" };
	actionClips[ActionType::DASH] = { "Dash" };
	actionClips[ActionType::SKILL] = { "" };	//스킬 클립은 각 Skill이 가짐
	actionClips[ActionType::SWITCH] = { "Switch" };
}

void BasicSkul::CreateComboAttacks(Animator* animator)
{
	/*
	string clipName;
	float moveDistance;
	float moveDuration;

	Vector2 hitboxSize;
	Vector2 hitboxOffset = Vector2::Zero;

	int damage;
	*/

	//기본 공격 콤보 스텝마다 클립 이름 + 이동거리/지속시간을 한 데이터로 묶어서 보관
	basicAttackCombos =
	{
		{ "AttackA", 30.0f, 0.10f, 8, AttackType::Melee,
		animator->FindClip(tag + "AttackA")->GetFrame(2)->GetSize()},

		{ "AttackB", 60.0f, 0.10f, 10, AttackType::Melee,
		animator->FindClip(tag + "AttackB")->GetFrame(1)->GetSize()},
	};

	jumpAttack = { "JumpAttack", 0.0f, 0.0f, 8, AttackType::Melee,
		animator->FindClip(tag + "JumpAttack")->GetFrame(1)->GetSize() };
}

void BasicSkul::CreateEffects(Animator* animator)
{
}

void BasicSkul::SetEffectDetails()
{
}

void BasicSkul::SetStats()
{
	stat = owner->GetStatComponent();

	//공통 스탯(Health, MovementSpeed)은 Player::SetStats()로 이전
	//여기에는 이 스컬 전용 보너스만 source = this로 추가 (교체 시 RemoveStat(this)로 제거)

	/*
	StatType type;

	Attribute attribute;
	float value;
	void* source = nullptr;
	*/
}

void BasicSkul::SetImages()
{
	mainPortrait = Texture::Add(L"ProjectTextures/Skul_Images/Skul_1.png");
	subPortrait = Texture::Add(L"ProjectTextures/Skul_Images/Skul.png");
	infoPortrait = Texture::Add(L"ProjectTextures/Skul_Images/Skul_2.png");
}

void BasicSkul::CreateSkills()
{
	int index = 0;

	throwSkill = new BasicSkullSkill1(owner, 0.0f, this);
	Skill* newSkill = throwSkill;
	newSkill->SetIndex(index++);
	newSkill->Init();
	newSkill->SetIcon();
	skills.push_back(newSkill);

	newSkill = new BasicSkullSkill2(owner, 0.0f, this);
	newSkill->SetIndex(index++);
	newSkill->Init();
	newSkill->SetIcon();
	skills.push_back(newSkill);

}

void BasicSkul::CreateHead()
{
	//1번 스킬(던지기)과 2번 스킬(머리 위치로 이동)이 같은 머리를 참조해야 해서 스킬이 아니라 BasicSkul이 소유한다
	head = new SkulObject(L"ProjectTextures/Skul_Skill/Skul_Skill.png");
	head->Scale() = SKUL_DEFAULT_ASSET_SIZE;
	head->SetMass(10.0f);
	head->SetDamping(5.0f);
	head->SetActive(false);

	Collider* col = new RectCollider(head->GetSize());
	col->SetParent(head);
	col->UpdateWorld();
	head->SetCollider(col);
}

void BasicSkul::RegisterHeadToStage(Stage* stage)
{
	if (headRegistered) return;

	stage->RegisterObject(head);	//head를 Stage에 등록해서 매 프레임 갱신/충돌 처리를 시킴
	headRegistered = true;
}
