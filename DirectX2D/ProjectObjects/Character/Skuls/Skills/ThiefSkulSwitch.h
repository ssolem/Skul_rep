#pragma once
#include "../../Skill.h"

class IHittable;

class ThiefSkulSwitch : public Skill
{
public:
	ThiefSkulSwitch(Character* owner);
	~ThiefSkulSwitch() override;

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
	const string slashClip = "AttackB";
	const int HITBOX_FRAME = 2;			
	const float MOVE_DISTANCE = 150.0f;

	Collider* attackCollider = nullptr;
	vector<IHittable*> alreadyHit;

	float damage = 20.0f;

	float backDir = 0.0f;
	float moveSpeed = 0.0f;
	float movedDistance = 0.0f;
};
