#include "Framework.h"
#include "SkulClass.h"
#include "Skill.h"

SkulClass::SkulClass()
{
	curSkills.resize(2);
}

SkulClass::~SkulClass()
{
	for (Skill* skill : skills)
		if (skill != nullptr)
			delete skill;

	delete switchSkill;
}

void SkulClass::Update()
{
	//이어지는 효과는 현재 스컬일 때만
	for (Skill* skill : curSkills)
	{
		if (skill) skill->UpdateEffect();
	}
}

void SkulClass::UpdateCoolDown()
{
	//장착한 스킬 쿨다운은 보조 스컬이어도 계속 감소
	for (Skill* skill : curSkills)
	{
		if (skill) skill->UpdateCoolDown();
	}
}

void SkulClass::OnSwitchOut()
{
	for (Skill* skill : skills)
	{
		if (skill) skill->OnSwitchOut();
	}
}

void SkulClass::Render()
{
}

void SkulClass::CreateEffects(Animator* animator)
{

}

void SkulClass::SetEffectDetails()
{

}