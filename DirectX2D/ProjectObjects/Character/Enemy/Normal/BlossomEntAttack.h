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
	void UpdateEffect() override;
	void Cancel() override;
	void Render() override;

private:
	void EndAction();	//몸의 공격 행동만 종료 (AI가 다음 행동으로 넘어감)
	void EndEffect();	//장판 종료
	HitData MakeHitData();

private:
	AttackData data;
	int effectFrame;

	AnimObject* blossom;
	RectCollider* hitbox;
	Vector2 castPos;
	bool alreadyHit = false;
	bool isCasted = false;	//장판이 깔렸으면 피격돼도 장판 유지
};

