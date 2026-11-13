#include "Framework.h"
#include "ThiefSkul.h"
#include "../Animator.h"
#include "../Character.h"
#include "../Skill.h"
#include "Skills/ThiefBackRoll.h"
#include "Skills/ThiefGoldAttack.h"
#include "Skills/ThiefBackAttack.h"
#include "Skills/ThiefPocketTear.h"
#include "Skills/ThiefSkulSwitch.h"
//#include "ProjectUtilities/ScopedTimer.h"

namespace
{
	//단계별로 달라지는 값만 모아 둠
	struct ThiefTierData
	{
		string tag;
		wstring atlas;
		float attackA, attackB;

		wstring mainPortrait, subPortrait, infoPortrait;
		wstring dropImage;	//Head 폴더 - 바닥에 떨어진 머리
	};

	const ThiefTierData tiers[] =
	{
		{ "Thief_1", L"ProjectTextures/Thief/Thief_Thief/Atlas", 30.0f, 60.0f,
			L"ProjectTextures/Thief/UI_Icons/Thief_1.png", L"ProjectTextures/Thief/UI_Icons/Thief_2.png", L"ProjectTextures/Thief/UI_Icons/Thief.png",
			L"ProjectTextures/Thief/Head/Thief.png" },
		{ "Thief_2", L"ProjectTextures/Thief/Thief_2/Atlas",     30.0f, 60.0f,
			L"ProjectTextures/Thief/UI_Icons/Thief_2_1.png", L"ProjectTextures/Thief/UI_Icons/Thief_2_3.png", L"ProjectTextures/Thief/UI_Icons/Thief_2_2.png",
			L"ProjectTextures/Thief/Head/Thief_2.png" },
		{ "Thief_3", L"ProjectTextures/Thief/Thief_3/Atlas",     35.0f, 40.0f,
			L"ProjectTextures/Thief/UI_Icons/Thief_3_2.png", L"ProjectTextures/Thief/UI_Icons/Thief_3_1.png", L"ProjectTextures/Thief/UI_Icons/Thief_3.png",
			L"ProjectTextures/Thief/Head/Thief_3.png" },
		{ "Thief_4", L"ProjectTextures/Thief/Thief_4/Atlas",     40.0f, 45.0f,
			L"ProjectTextures/Thief/UI_Icons/Thief_4_2.png", L"ProjectTextures/Thief/UI_Icons/Thief_4.png", L"ProjectTextures/Thief/UI_Icons/Thief_4_1.png",
			L"ProjectTextures/Thief/Head/Thief_4.png" },
	};
	const int tierCount = sizeof(tiers) / sizeof(tiers[0]);

	const int tierUpgradeCost[tierCount - 1] = { 30, 40, 50 };
}

ThiefSkul::ThiefSkul()
{
	SetTag(tiers[0].tag);
	dropImage = tiers[0].dropImage;
	hitSoundKey = "Hit_Sword_Small";
}

ThiefSkul::~ThiefSkul()
{
}

void ThiefSkul::Update()
{
	SkulClass::Update();

}

void ThiefSkul::Render()
{
	SkulClass::Render();

}

void ThiefSkul::Init(Character* owner, Animator* animator)
{
	this->owner = owner;
	this->animator = animator;

	{ /*ScopedTimer t("CreateSounds");*/     CreateSounds(); }
	{ /*ScopedTimer t("CreateAnimations");*/ CreateAnimations(animator); }
	{ /*ScopedTimer t("CreateEffects");*/    CreateEffects(animator); }	//뒤구르기 연막/가스 이펙트 등록
	SetAnimations();
	SetStats();
	{ /*ScopedTimer t("CreateSkills");*/     CreateSkills(); }
	{ /*ScopedTimer t("ApplyTier");*/        ApplyTier(tier); }

	curSkills[0] = skills[0];
	curSkills[1] = skills[3];

	initialized = true;
}

void ThiefSkul::CreateAnimations(Animator* animator)
{
	const char* onceClips[] = { "AttackA", "AttackB", "Dash", "Fall", "Jump", "JumpAttack",
		"Skill1_Ready", "Skill1_Attack", "Skill2", "Skill3_Ready", "Skill3_Attack" };
	const char* loopClips[] = { "Idle", "Walk", "FallRepeat" };

	for (const ThiefTierData& data : tiers)
	{
		for (const char* name : onceClips)
			animator->CreateClipByAtlas(data.tag, name, data.atlas, false);
		for (const char* name : loopClips)
			animator->CreateClipByAtlas(data.tag, name, data.atlas);

		//키를 값으로 캡처해서 단계마다 자기 FallRepeat로 연결

		string repeatKey = data.tag + "FallRepeat";
		animator->FindClip(data.tag + "Fall")->SetEvent([animator, repeatKey]() { animator->PlayClip(repeatKey); });

		Clip* jumpAttack = animator->FindClip(data.tag + "JumpAttack");
		jumpAttack->SetEvent([]() { Audio::Get()->Play("Thief_SlashUp"); }, jumpAttack->GetFrameCount() / 2);
	}
}

void ThiefSkul::LoadResources(Animator* animator)
{
	SkulClass::LoadResources(animator);

	for (const ThiefTierData& data : tiers)
	{
		Texture::Add(data.mainPortrait);
		Texture::Add(data.subPortrait);
		Texture::Add(data.infoPortrait);
		Texture::Add(data.dropImage);
	}

	Animator::GetAtlasFrames(L"ProjectTextures/Thief/Projectile\\Thief_Caltrap.png");
	Animator::GetAtlasFrames(L"ProjectTextures/Thief/Projectile\\Thief_GoldExplosion_Projectile.png");

	Texture::Add(L"ProjectTextures/Thief/UI_Skill_Icons/BackRoll.png");
	Texture::Add(L"ProjectTextures/Thief/UI_Skill_Icons/GoldExplosion.png");
	Texture::Add(L"ProjectTextures/Thief/UI_Skill_Icons/Backattack.png");
	Texture::Add(L"ProjectTextures/Thief/UI_Skill_Icons/PocketTear.png");
}

void ThiefSkul::ApplyTier(int newTier)
{
	tier = newTier < 0 ? 0 : (newTier >= tierCount ? tierCount - 1 : newTier);
	SetTag(tiers[tier].tag);

	//태그가 바뀌었으니 프레임 크기도 새 단계 기준으로 다시 계산
	for (Skill* skill : skills)
	{
		skill->ApplyTier(tier);	
	}
	CreateComboAttacks(animator);
	SetImages();
}

bool ThiefSkul::IsMaxTier() const
{
	return tier >= tierCount - 1;
}

int ThiefSkul::GetUpgradeCost() const
{
	return IsMaxTier() ? 0 : tierUpgradeCost[tier];
}

void ThiefSkul::UpgradeTier()
{
	if (!IsMaxTier()) ApplyTier(tier + 1);
}

void ThiefSkul::SetAnimations()
{
	actionClips[ActionType::JUMP_ATTACK] = { "JumpAttack" };
	actionClips[ActionType::DASH] = { "Dash" };
	actionClips[ActionType::SKILL] = { "" };
	actionClips[ActionType::SWITCH] = { "AttackB" };
}

void ThiefSkul::CreateEffects(Animator* animator)
{
	animator->CreateEffectByAtlas("ThiefSmoke", L"ProjectTextures/Thief/Thief_BackRoll/Thief_BackRoll.png", 1, SKUL_DEFAULT_ASSET_SIZE, 1.0f, false, EffectPivot::Bottom);
	animator->CreateEffectByAtlas("ThiefPlague", L"ProjectTextures/Thief/Effect_BackRoll_Smoke_PlagueDoctor_Gas/PlagueDoctor_Gas.png", 1, SKUL_DEFAULT_ASSET_SIZE, 1.0f, false, EffectPivot::Center);
	animator->CreateEffectByAtlas("ThiefGoldExplosion", L"ProjectTextures/Thief/Thief_GoldExplosion/Thief_GoldExplosion.png", 1, SKUL_DEFAULT_ASSET_SIZE, 1.0f, false, EffectPivot::Center);
	animator->CreateEffectByAtlas("ThiefBackAttack", L"ProjectTextures/Thief/Thief_BackAttack/Thief_BackAttack.png", 1, SKUL_DEFAULT_ASSET_SIZE, 1.0f, false, EffectPivot::Center);	
	animator->CreateEffectByAtlas("Thief4BackAttack", L"ProjectTextures/Thief/Thief_4_BackAttack/Thief_4_BackAttack.png", 1, SKUL_DEFAULT_ASSET_SIZE, 1.0f, false, EffectPivot::Center);	
	animator->CreateEffectByAtlas("ThiefPocketTear", L"ProjectTextures/Thief/Thief_PocketTear_Slash/Thief_PocketTear_Slash.png", 1, SKUL_DEFAULT_ASSET_SIZE, 1.0f, false, EffectPivot::Center);	
	animator->CreateEffectByAtlas("Thief4PocketTear", L"ProjectTextures/Thief/Thief_4_PocketTear/Thief_4_PocketTear.png", 1, SKUL_DEFAULT_ASSET_SIZE, 1.0f, false, EffectPivot::Center);	
	animator->CreateEffectByAtlas("ThiefPocketTearSmoke", L"ProjectTextures/Thief/Thief_PocketTear/Thief_PocketTear.png", 1, SKUL_DEFAULT_ASSET_SIZE, 1.0f, false, EffectPivot::Center);	

}

void ThiefSkul::SetEffectDetails()
{
}

void ThiefSkul::SetStats()
{
	stat = owner->GetStatComponent();
}

void ThiefSkul::SetImages()
{
	const ThiefTierData& data = tiers[tier];

	mainPortrait = Texture::Add(data.mainPortrait);
	subPortrait = Texture::Add(data.subPortrait);
	infoPortrait = Texture::Add(data.infoPortrait);
	dropImage = data.dropImage;
}

void ThiefSkul::CreateSkills()
{
	int index = 0;

	Skill* newSkill = new ThiefBackRoll(owner, this);
	newSkill->SetIndex(index++);
	newSkill->Init();
	newSkill->SetIcon();
	skills.push_back(newSkill);

	newSkill = new ThiefGoldAttack(owner, this);
	newSkill->SetIndex(index++);
	newSkill->Init();
	newSkill->SetIcon();
	skills.push_back(newSkill);

	newSkill = new ThiefBackAttack(owner, this);
	newSkill->SetIndex(index++);
	newSkill->Init();
	newSkill->SetIcon();
	skills.push_back(newSkill);

	newSkill = new ThiefPocketTear(owner, this);
	newSkill->SetIndex(index++);
	newSkill->Init();
	newSkill->SetIcon();
	skills.push_back(newSkill);

	switchSkill = new ThiefSkulSwitch(owner);
	switchSkill->Init();
}

void ThiefSkul::CreateComboAttacks(Animator* animator)
{
	const ThiefTierData& data = tiers[tier];

	basicAttackCombos =
	{
		{ "AttackA", data.attackA, 0.10f, 9, AttackType::Melee,
		animator->FindClip(tag + "AttackA")->GetFrame(2)->GetSize(), Vector2::Zero, "Thief_Atk1"},

		{ "AttackB", data.attackB, 0.10f, 9, AttackType::Melee,
		animator->FindClip(tag + "AttackB")->GetFrame(2)->GetSize(), Vector2::Zero, "Thief_Atk2"},
	};

	jumpAttack = { "JumpAttack", 0.0f, 0.0f, 8, AttackType::Melee,
		animator->FindClip(tag + "JumpAttack")->GetFrame(1)->GetSize(), Vector2::Zero, "Thief_JumpAtk" };
}

//효과음 등록 - 키가 이미 있으면 Audio::Add가 무시 (BasicSkul과 파일명이 겹쳐서 Thief_ 접두어)
void ThiefSkul::CreateSounds()
{
	const string path = "ProjectSounds/Thief/";

	Audio::Get()->Add("Thief_Atk1", path + "Atk_Sword_Small 1.wav");
	Audio::Get()->Add("Thief_Atk2", path + "Atk_Sword_Small 2.wav");
	Audio::Get()->Add("Thief_JumpAtk", path + "Atk_BluntWeapon_Small 2.wav");
	Audio::Get()->Add("Thief_SlashUp", path + "Atk_Sword_Large (Slash Up).wav");
	Audio::Get()->Add("Thief_SlashDown", path + "Atk_Sword_Large (Slash Down).wav");
	Audio::Get()->Add("Thief_Sword4", path + "Atk_Sword_Small 4.wav");
	Audio::Get()->Add("Thief_Sword5", path + "Atk_Sword_Small 5.wav");
	Audio::Get()->Add("Thief_SmokeSmall", path + "Atk_Smoke_Small.wav");
	Audio::Get()->Add("Thief_SmokeMedium", path + "Atk_Smoke_Medium.wav");
	Audio::Get()->Add("Thief_Roll", path + "Atk_BluntWeapon_Large.wav");
	Audio::Get()->Add("Thief_GoldThrow", path + "Atk_BluntWeapon_Small 1.wav");
	Audio::Get()->Add("Thief_GainGold", path + "Object_GainGold.wav");
	Audio::Get()->Add("Thief_GoldExplode", path + "Atk_Explosion_Small.wav");
}
