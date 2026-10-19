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

	void SetSkill(Skill* skill) { nextSkill = skill; }

private:

	bool isAttacking;
	Skill* curSkill;				//지금 실행 중인 스킬 (End/Update/Render 대상)
	Skill* nextSkill = nullptr;		//다음에 시작할 스킬
};

