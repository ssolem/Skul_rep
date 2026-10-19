#pragma once
#include "../../Skill.h"
class ThiefSkul;
class Enemy;

//주머니 찢기 - Skill1_Ready -> Skill1_Attack, 앞쪽 베기
class ThiefPocketTear : public Skill
{
public:
	ThiefPocketTear(Character* owner, ThiefSkul* thiefSkul);
	~ThiefPocketTear() override;

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
	void OnReadyEnd();					//준비 모션 끝 -> 베기
	void Slash();
	HitData CreateHitData(float attackDir);

private:
	struct TierData
	{
		float damage;
		float coolDown;
		int goldPerHit;				//적중한 적마다 얻는 골드 
		float goldChance;			//4단계 - 추가 골드 확률
		float additionalGoldHitTime = 0.0f;		//x초 이내 적 타격시 골드 획득
		string effectKey;
		Vector2 slashSize;			//판정 크기
		DamageType damageType = DamageType::Magic;
		AttackType attackType = AttackType::Melee;
	};

	vector<TierData> tierDatas;
	TierData curData;

	ThiefSkul* thiefSkul;
	Collider* hitbox;

	const string smokeKey = "ThiefPocketTearSmoke";
	const string readyClip = "Skill1_Ready";
	const string attackClip = "Skill1_Attack";
};

