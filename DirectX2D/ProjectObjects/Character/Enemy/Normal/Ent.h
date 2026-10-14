#pragma once
#include "Enemy.h"

class Animator;

class Ent : public Enemy
{
public:
	Ent();
	~Ent() override;

protected:
	virtual void CreateAnimations(Animator* animator) override;
	virtual void SetStats() override;
	virtual void InitBody() override;
	virtual void CreateAttacks(EnemyAI* ai) override;
};

