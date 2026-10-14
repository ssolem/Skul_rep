#pragma once
#include "../../Skill.h"

class BasicSkul;

class BasicSkullSkill2 : public Skill
{
public:
	BasicSkullSkill2(Character* owner, float coolDown, BasicSkul* basicSkul);
	~BasicSkullSkill2() override;

	virtual void Init() override;

	virtual void Start() override;
	virtual void End() override;
	virtual bool IsReady() override;
	virtual void SetIcon() override;

private:
	SkulObject* head = nullptr;
	BasicSkul* basicSkul = nullptr;
};

