#pragma once
#include "EnemyAttack.h"

//근접 휘두르기
class MeleeSwing : public EnemyAttack
{
public:
	MeleeSwing(Enemy* owner, const AttackData& data, int hitFrame, float coolDown);

	void Start(Character* target) override;
	void Update() override;
	void Cancel() override;
	void Render() override;

private:
	void Finish();
	HitData MakeHitData();

private:
	AttackData data;
	int hitFrame;
	Collider* hitbox;
	bool alreadyHit = false;
};
