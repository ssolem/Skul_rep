#pragma once
#include "../../Skill.h"
class ThiefSkul;
class IHittable;

class ThiefGoldAttack : public Skill
{
public:
	ThiefGoldAttack(Character* owner, ThiefSkul* thiefSkul);
	~ThiefGoldAttack() override;

	virtual void Init() override;
	virtual void Update() override;
	virtual void UpdateCoolDown() override;
	virtual void Render() override;

	virtual void Start() override;
	virtual void End() override;
	virtual bool IsReady() override;
	virtual void SetIcon() override;

	virtual void ApplyTier(int newTier) override;

private:
	HitData CreateHitData(float attackDir);
	void CreateGoldProjectile();
	void FireGoldProjectile();

private:
	struct TierData
	{
		float damage;
		float coolDown;
		int cost;
		DamageType damageType;
		int penetrateCount = 0;
		AttackType attackType = AttackType::Projectile;
	};

	vector<TierData> tierDatas;
	TierData curData;
	vector<IHittable*> alreadyHit;

	ThiefSkul* thiefSkul;

	float goldSpeed = 600.0f;
	const string goldTag = "ThiefGold";

};
