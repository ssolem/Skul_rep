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

namespace
{
	//단계별로 달라지는 값만 모아 둠

	struct ThiefTierData
	{
		string tag;
		wstring atlas;
		float attackA, attackB;

		//UI_Icons 파일명이 단계마다 규칙이 달라서 크기 기준으로 짝지음 (main 45x45, sub 21x21, info 높이 68~76)

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
}

ThiefSkul::ThiefSkul()
{
	SetTag(tiers[0].tag);
	dropImage = tiers[0].dropImage;
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

	CreateAnimations(animator);
	CreateEffects(animator);	//뒤구르기 연막/가스 이펙트 등록
	SetAnimations();
	SetStats();
	CreateSkills();
	ApplyTier(tier);

	//TODO: 1번 스킬 생성 후 curSkills[1] 연결 (비어 있으면 SkillState::IsReady가 false라 사용만 안 됨)

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
	}
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
		animator->FindClip(tag + "AttackA")->GetFrame(2)->GetSize()},

		{ "AttackB", data.attackB, 0.10f, 9, AttackType::Melee,
		animator->FindClip(tag + "AttackB")->GetFrame(2)->GetSize()},
	};

	jumpAttack = { "JumpAttack", 0.0f, 0.0f, 8, AttackType::Melee,
		animator->FindClip(tag + "JumpAttack")->GetFrame(1)->GetSize() };
}
