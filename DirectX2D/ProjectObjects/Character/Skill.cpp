#include "Framework.h"
#include "Skill.h"

Skill::Skill(Character* owner, float coolDown)
	:owner(owner), coolDownMax(coolDown), coolDownTime(0.0f), index(0), usedSkill(false), isReady(true)
{
}

Skill::~Skill()
{
}

void Skill::Update()
{

}

void Skill::UpdateCoolDown()
{
	if (usedSkill)
	{
		if (coolDownTime <= 0.0f)
		{
			usedSkill = false;
			isReady = true;
			coolDownTime = coolDownMax;
			return;
		}
		coolDownTime -= DELTA;
	}
}

void Skill::Render()
{
}

void Skill::Start()
{
}

void Skill::End()
{
}

bool Skill::IsReady()
{
	return isReady;
}
