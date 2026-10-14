#pragma once
#include "Enemy.h"
class RootEnt : public Enemy
{
public:
	RootEnt();
	~RootEnt() override;

protected:
	virtual void CreateAnimations(Animator* animator) override;
	virtual void SetStats() override;
	virtual void InitBody() override;
	virtual void CreateAttacks(EnemyAI* ai) override;
};

