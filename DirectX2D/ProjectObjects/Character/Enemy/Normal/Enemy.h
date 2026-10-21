#pragma once
#include "../../Character.h"
#include "ProjectObjects/Objects/DropTable.h"

class EnemyHealthBar;
class EnemyAI;

class Enemy : public Character
{
public:
	Enemy();
	~Enemy() override;

	void Init();   // CreateAnimations/SetStats가 순수가상이라 생성자에서 못 부름 - Ent가 자기 생성자에서 완전히 구성된 뒤 호출

	virtual void PlayClip(const string& name) override;
	virtual Clip* FindClip(const string& name) override;
	virtual void Render() override;

	//AI가 쓰는 동사 - 판단은 AI, 실제 이동/방향 전환은 Enemy
	void MoveToward(float dirX);		//낭떠러지/벽 앞이면 방향만 돌리고 정지
	void FaceTo(Character* target);		//이동 없이 타겟 쪽으로 돌아봄
	void Spawn(Vector2 pos);

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
	DropTable dropTable;	//SetStats()에서 적마다 채움 - 죽을 때 떨어뜨림

private:
	void SetHPBar();
	void Appear(Vector2 pos) { Pos() = pos; SetActive(true); }

private:
	Collider* frontSensor = nullptr;
	EnemyAI* ai = nullptr;

	EnemyHealthBar* hpBar = nullptr;
	Vector2 hpBarOffset = Vector2::Zero;

	float stunTime = 0.0f;	//남은 기절 시간 - 그동안 AI 판단 정지
	float goldMarkTime = 0.0f;	//골드 표식 남은 시간
};

