#pragma once
#include "EnemyAttack.h"

class AnimObject;

class BlossomEntAttack : public EnemyAttack
{
public:
	BlossomEntAttack(Enemy* owner, float coolDown, float range);
	~BlossomEntAttack();

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

	AnimObject* blossom;
	RectCollider* hitbox;
	Vector2 castPos;
	bool alreadyHit = false;
};

