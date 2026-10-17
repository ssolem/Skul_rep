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
}

void SkulClass::Update()
{
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