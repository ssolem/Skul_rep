#pragma once
#include "ActionState.h"
class JumpAttackState : public ActionState
{
public:

	JumpAttackState(Character* character, ActionType type, const AttackData& data, const vector<string>& clipNames);
	~JumpAttackState() override;

	virtual void Update() override;
	virtual void UpdateCoolDown() override;
	virtual void Render() override;

	virtual void Start() override;
	virtual void End() override;

	virtual bool IsReady() override;

private:
	void CheckHit();

private:
	Collider* attackCollider = nullptr;
	vector<IHittable*> alreadyHit;

	bool isAttacking;

	AttackData data;
	DamageType damageType;  

};

