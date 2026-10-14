#pragma once
#include "EnemyAttack.h"
class AnimObject;
class Animator;

class GiganticEntRange : public EnemyAttack
{
public:
	GiganticEntRange(Enemy* owner, int projectileFrame, float coolDown, float range);
	~GiganticEntRange();

	void Start(Character* target) override;
	void Update() override;
	void Cancel() override;
	void Render() override;

private:
	void Finish();
	HitData MakeHitData();
	void CreateProjectile();
	void Fire();

private:
	AttackData data;
	int projectileFrame;

	RectCollider* hitbox;
	Vector2 castPos;
	bool alreadyHit = false;

	int projectileCount = 6;
};

