#pragma once
#include "EnemyAttack.h"

class AnimObject;

class RootEntAttack : public EnemyAttack
{
public:
	RootEntAttack(Enemy* owner, int spikeFrame, float coolDown, float range);
	~RootEntAttack();

	void Start(Character* target) override;
	void Update() override;
	void Cancel() override;
	void Render() override;

private:
	void Finish();
	HitData MakeHitData();

private:
	AttackData data;
	int spikeFrame;				//RootEnt Attack 클립에서 뿌리가 솟는 프레임
	int hitStartFrame = 0;		//spike 클립에서 판정 켜지는 프레임
	int hitEndFrame = 3;		//spike 클립에서 판정 꺼지는 프레임

	AnimObject* sign;
	AnimObject* spike;
	RectCollider* hitbox;		//spike의 자식 - 바닥 기준, Scale은 spike 것을 따름
	Vector2 castPos;			
	Vector2 signOffset;
	bool alreadyHit = false;
};
