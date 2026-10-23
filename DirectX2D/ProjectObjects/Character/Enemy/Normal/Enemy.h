#pragma once
#include "../../Character.h"
#include "ProjectObjects/Objects/DropTable.h"

class EnemyHealthBar;
class EnemyAI;

class Enemy : public Character
{
public:
	enum class State
	{
		Pending,	//아직 Spawn 전
		Spawning,	//비활성이지만 남은 적
		Alive,
		Dead
	};

public:
	Enemy();
	~Enemy() override;

	void Init(); 

	virtual void PlayClip(const string& name) override;
	virtual Clip* FindClip(const string& name) override;
	virtual void Render() override;

	void MoveToward(float dirX);		//낭떠러지/벽 앞이면 방향만 돌리고 정지
	void FaceTo(Character* target);		//이동 없이 타겟 쪽으로 돌아봄
	void Spawn(Vector2 pos);

	State GetState() { return state; }
	bool IsRemaining() { return state == State::Spawning || state == State::Alive; }	//웨이브/남은 적 카운트 기준

protected:

	virtual void UpdateControl() override;
	virtual void UpdateCollision() override;
	virtual void PostUpdate() override;
	virtual void InitBody() override;
	virtual void CreateAnimations(Animator* animator) = 0;
	virtual void SetStats() = 0;
	virtual void CreateAttacks(EnemyAI* ai) = 0;	//적마다 공격 패턴 조립

	//interface
	virtual void OnHit(const HitData& hit) override;
	virtual void Die() override;

	virtual void StartKnockBack(const HitData& hit) override;
	virtual void EndKnockBack() override;

	bool CannotMoveForward();

protected:
	DropTable dropTable;

private:
	void SetHPBar();
	void Appear(Vector2 pos);
	void CancelSpawnEffect();

private:
	State state = State::Pending;
	Effect* spawnEffect = nullptr;

	Collider* frontSensor = nullptr;
	EnemyAI* ai = nullptr;

	EnemyHealthBar* hpBar = nullptr;
	Vector2 hpBarOffset = Vector2::Zero;

	float stunTime = 0.0f;	
	float goldMarkTime = 0.0f;	//골드 표식 남은 시간
};

