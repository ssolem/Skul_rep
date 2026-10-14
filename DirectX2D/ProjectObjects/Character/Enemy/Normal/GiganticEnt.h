#pragma once
#include "Enemy.h"

class GiganticEnt : public Enemy
{
public:
	GiganticEnt();
	~GiganticEnt() override;

protected:
	virtual void CreateAnimations(Animator* animator) override;
	virtual void SetStats() override;
	virtual void InitBody() override;
	virtual void CreateAttacks(EnemyAI* ai) override;
};

