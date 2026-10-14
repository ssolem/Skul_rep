#pragma once
#include "ActionState.h"

class Skill;

class SkillState : public ActionState
{
public:

	SkillState(Character* character, ActionType type, const vector<string>& clipNames);
	~SkillState() override;

	virtual void Update() override;
	virtual void UpdateCoolDown() override;
	virtual void Render() override;

	virtual void Start() override;
	virtual void End() override;

	virtual bool IsReady() override;

	void SetSkill(Skill* skill) { curSkill = skill; }

private:

	bool isAttacking;
	Skill* curSkill;
};

