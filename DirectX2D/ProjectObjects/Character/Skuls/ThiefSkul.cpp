#include "Framework.h"
#include "ThiefSkul.h"
#include "../Animator.h"
#include "../Character.h"

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
	};

	const ThiefTierData tiers[] =
	{
		{ "Thief_1", L"ProjectTextures/Thief/Thief_Thief/Atlas", 30.0f, 60.0f,
			L"ProjectTextures/Thief/UI_Icons/Thief_1.png", L"ProjectTextures/Thief/UI_Icons/Thief_2.png", L"ProjectTextures/Thief/UI_Icons/Thief.png" },
		{ "Thief_2", L"ProjectTextures/Thief/Thief_2/Atlas",     30.0f, 60.0f,
			L"ProjectTextures/Thief/UI_Icons/Thief_2_1.png", L"ProjectTextures/Thief/UI_Icons/Thief_2_3.png", L"ProjectTextures/Thief/UI_Icons/Thief_2_2.png" },
		{ "Thief_3", L"ProjectTextures/Thief/Thief_3/Atlas",     35.0f, 40.0f,
			L"ProjectTextures/Thief/UI_Icons/Thief_3_2.png", L"ProjectTextures/Thief/UI_Icons/Thief_3_1.png", L"ProjectTextures/Thief/UI_Icons/Thief_3.png" },
		{ "Thief_4", L"ProjectTextures/Thief/Thief_4/Atlas",     40.0f, 45.0f,
			L"ProjectTextures/Thief/UI_Icons/Thief_4_2.png", L"ProjectTextures/Thief/UI_Icons/Thief_4.png", L"ProjectTextures/Thief/UI_Icons/Thief_4_1.png" },
	};
	const int tierCount = sizeof(tiers) / sizeof(tiers[0]);
}

ThiefSkul::ThiefSkul()
{
	SetTag(tiers[0].tag);
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
	SetAnimations();
	SetStats();
	CreateSkills();
	ApplyTier(0);

	//TODO: CreateSkills() 구현 후 curSkills[0], curSkills[1] 연결 (지금은 skills가 비어 있어서 접근하면 크래시)

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

	CreateComboAttacks(animator);
	SetImages();
}

void ThiefSkul::SetAnimations()
{
	actionClips[ActionType::JUMP_ATTACK] = { "JumpAttack" };
	actionClips[ActionType::DASH] = { "Dash" };
	actionClips[ActionType::SKILL] = { "Skill" };
	actionClips[ActionType::SWITCH] = { "AttackB" };
}

void ThiefSkul::CreateEffects()
{
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
}

void ThiefSkul::CreateSkills()
{
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
