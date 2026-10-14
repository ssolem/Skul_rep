#pragma once
#include "EnemyAttack.h"

class AnimObject;

class GiganticEntMelee : public EnemyAttack
{
public:
	GiganticEntMelee(Enemy* owner, int effectFrame, float coolDown, float range);
	~GiganticEntMelee();

	void Start(Character* target) override;
	void Update() override;
	void Cancel() override;
	void Render() override;

private:
	void Finish();
	HitData MakeHitData();

private:
	AttackData data;
	int effectFrame;
	int hitStartFrame = 0;
	int hitEndFrame = 3;

	AnimObject* stamp;
	RectCollider* hitbox;
	Vector2 castPos;		
	bool alreadyHit = false;
	float launchHeight = 80.0f;
};

