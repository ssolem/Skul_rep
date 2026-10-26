#pragma once
#include "../../Skill.h"

class IHittable;

class BasicSkulSwitch : public Skill
{
public:
	BasicSkulSwitch(Character* owner);
	~BasicSkulSwitch() override;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;

	virtual void Start() override;
	virtual void End() override;
	virtual void SetIcon() override {}

private:
	void Move();
	void CheckHit();

private:
	const string spinClip = "Switch";	
	const float SPIN_COUNT = 2.5;
	const int HITBOX_FRAME = 2;		
	const float MOVE_DISTANCE = 500.0f;

	Collider* attackCollider = nullptr;	
	vector<IHittable*> alreadyHit;
	Clip* clip = nullptr;

	float damage = 10.0f;

	float moveDir = 0.0f;
	float moveSpeed = 0.0f;
	float totalTime = 0.0f;		
	float halfTurnTime = 0.0f;	
	float timer = 0.0f;
	float halfTurnTimer = 0.0f;
};
