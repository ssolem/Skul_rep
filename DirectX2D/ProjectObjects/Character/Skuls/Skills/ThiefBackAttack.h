#pragma once
#include "../../Skill.h"
class ThiefSkul;
class Enemy;

//뒤치기 - Skill3_Ready -> Skill3_Attack
class ThiefBackAttack : public Skill
{
public:
	ThiefBackAttack(Character* owner, ThiefSkul* thiefSkul);
	~ThiefBackAttack() override;

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
	void OnReadyEnd();					//준비 모션 끝 -> 순간이동 + 베기
	void TeleportBehind(Enemy* enemy);
	void Slash();
	HitData CreateHitData(float attackDir);

private:
	struct TierData
	{
		float damage;
		float coolDown;
		int goldPerHit;				//적중한 적마다 얻는 골드 
		bool invincible;			//시전 중 무적
		bool teleport;				//가장 가까운 적 뒤로 순간이동
		float stunChance;			//기절 확률
		bool alwaysCritical;		//항상 치명타
		string effectKey;
		Vector2 slashSize;			//베기 판정 크기
		DamageType damageType = DamageType::Magic;
		AttackType attackType = AttackType::Melee;
	};

	vector<TierData> tierDatas;
	TierData curData;

	ThiefSkul* thiefSkul;
	Collider* hitbox;

	float teleportRange = 400.0f;	//이 거리 안의 적에게만 순간이동
	float behindOffset = 40.0f;		//적 뒤로 떨어지는 거리
	float stunTime = 1.0f;


	const string readyClip = "Skill3_Ready";
	const string attackClip = "Skill3_Attack";
};
