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
	//장착한 스킬은 사용 여부와 상관없이 매 프레임 쿨다운/지속 효과 갱신
	for (Skill* skill : curSkills)
	{
		if (skill) skill->UpdateCoolDown();
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