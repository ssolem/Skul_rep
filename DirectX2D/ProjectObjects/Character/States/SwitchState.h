#pragma once
#include "ActionState.h"

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

private:

	bool isSwitching;
};

