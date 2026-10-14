#include "Framework.h"
#include "ThiefSkul.h"
#include "../Animator.h"
#include "../Character.h"

ThiefSkul::ThiefSkul()
{
	SetTag("ThiefSkul");
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

	CreateAnimations(animator);
	SetStats();
	SetImages();
	CreateSkills();

	curSkills[0] = skills[0];
}

void ThiefSkul::CreateAnimations(Animator* animator)
{
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

}

void ThiefSkul::SetAnimations()
{
}

void ThiefSkul::CreateEffects()
{
}

void ThiefSkul::SetEffectDetails()
{
}

void ThiefSkul::SetStats()
{
}

void ThiefSkul::SetImages()
{
}

void ThiefSkul::CreateSkills()
{
}

void ThiefSkul::CreateComboAttacks(Animator* animator)
{
}
