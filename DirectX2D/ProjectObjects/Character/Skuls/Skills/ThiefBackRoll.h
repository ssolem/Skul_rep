#pragma once
#include "../../Skill.h"
class ThiefSkul;
class IHittable;

class ThiefBackRoll : public Skill
{
public:
	ThiefBackRoll(Character* owner, ThiefSkul* thiefSkul);
	~ThiefBackRoll() override;

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
	void Move();
	void CheckHit();
	void PlaceHitbox(Vector2 size);

	void OnSmokeAction();
	void OnCaltrapAction();
	void OnPlagueAction();
	void CreateCaltrap();
	HitData CreateHitData(float attackDir);

private:
	enum class BackRollType
	{
		Smoke, Caltrap, Plague
	};

	struct TierData
	{
		float damage;
		float coolDown;
		int maxCharges;
		BackRollType smokeType;
		DamageType damageType;
		AttackType attackType = AttackType::Range;
	};

	vector<TierData> tierDatas;
	TierData curData;
	vector<IHittable*> alreadyHit;

	ThiefSkul* thiefSkul;
	Collider* hitbox;

	float backDir = 0.0f;
	float speed = 800.0f;
	float distance = 300.f;
	float movedDistance = 0.0f;

	int curCharges = 0;
	float skillUseCooldown = 0.0f;
	float skillUseMaxCooldown = 1.0f;
	float caltrapSpeed = 300.0f;
	int caltrapCount = 3;
	const string caltrapTag = "ThiefCaltrap";
};

