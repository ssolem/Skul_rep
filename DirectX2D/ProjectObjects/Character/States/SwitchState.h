#pragma once
#include "ActionState.h"

class Skill;

class SwitchState : public ActionState
{
public:

	SwitchState(Character* character, ActionType type, const vector<string>& clipNames);
	~SwitchState() override;

	virtual void Update() override;
	virtual void UpdateCoolDown() override;
	virtual void Render() override;

	virtual void Start() override;
	virtual void End() override;

	virtual bool IsReady() override;

	void SetSkill(Skill* skill) { switchSkill = skill; }

	float GetCoolDownRatio() { return 1.0f - max(0.0f, min(coolDownTime / COOL_DOWN_MAX, 1.0f)); }

private:
	const float COOL_DOWN_MAX = 8.0f;

	bool isSwitching;
	Skill* switchSkill = nullptr;
	float coolDownTime = 0.0f;
};

