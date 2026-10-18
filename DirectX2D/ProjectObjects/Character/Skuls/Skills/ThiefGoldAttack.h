#pragma once
#include "../../Skill.h"
class ThiefSkul;
class IHittable;
class Projectile;

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
	void GoldExplode(Vector2 pos);

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
	Collider* hitbox;

	float goldSpeed = 600.0f;
	const string goldTag = "ThiefGold";

	//연사
	int remainShots = 0;			//남은 발사 수
	float fireTimer = 0.0f;
	float fireInterval = 0.1f;
	float fireDir = 1.0f;			//시전 순간 바라보던 방향으로 고정
	float fireOffsetY = 10.0f;		//발사 높이 랜덤 범위 (±)

};
