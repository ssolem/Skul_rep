#pragma once
#include "../SkulClass.h"

class SkulObject;
class Stage;

class ThiefSkul : public SkulClass
{
public:
	ThiefSkul();
	virtual ~ThiefSkul() override;

	virtual void Update() override;
	virtual void Render() override;

	virtual void Init(Character* owner, Animator* animator) override;
	virtual void CreateAnimations(Animator* animator) override;
	virtual void SetAnimations() override;
	virtual void CreateEffects(Animator* animator) override;
	virtual void SetEffectDetails() override;
	virtual void SetStats() override;
	virtual void SetImages() override;

	virtual const vector<AttackData>& GetComboAttacks() const override { return basicAttackCombos; }

	void ApplyTier(int newTier);	//단계 변경: 태그, 콤보, 아이콘 교체

private:
	virtual void CreateSkills() override;
	virtual void CreateComboAttacks(Animator* animator) override;

private:
	Animator* animator = nullptr;
	int tier = 0;
};

