#pragma once
#include "ActionState.h"
class JumpAttackState : public ActionState
{
public:

	JumpAttackState(Character* character, ActionType type, const vector<string>& clipNames);
	~JumpAttackState() override;

	virtual void Update() override;
	virtual void UpdateCoolDown() override;
	virtual void Render() override;

	virtual void Start() override;
	virtual void End() override;

	virtual bool IsReady() override;

private:

	bool isAttacking;

};

