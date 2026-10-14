#pragma once
#include "Enemy.h"

class BlossomEnt : public Enemy
{
public:
	BlossomEnt();
	~BlossomEnt() override;

protected:
	virtual void CreateAnimations(Animator* animator) override;
	virtual void SetStats() override;
	virtual void InitBody() override;
	virtual void CreateAttacks(EnemyAI* ai) override;
};

