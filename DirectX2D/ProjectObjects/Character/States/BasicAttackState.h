#pragma once
#include "ActionState.h"

class BasicAttackState : public ActionState
{
public:

	BasicAttackState(Character* character, ActionType type, const vector<AttackData>& combos, DamageType damageType);
	~BasicAttackState() override;

	virtual void Update() override;
	virtual void UpdateCoolDown() override;
	virtual void Render() override;

	virtual void Start() override;
	virtual void End() override;

	virtual bool IsReady() override;

private:

	void AttackMovement();
	static vector<string> ExtractClipNames(const vector<AttackData>& combos);
	void CheckHit();

private:
	Collider* attackCollider = nullptr;
	vector<IHittable*> alreadyHit;

	bool finishedAttack;
	bool isAttacking;

	int maxComboIndex;
	int curComboIndex;
	int curAttackIndex = 0;

	vector<AttackData> combos;   // 콤보 스텝별 클립 이름 + 이동거리/지속시간 (Skul이 소유, 생성 시 복사)
	DamageType damageType;       // 이 캐릭터의 기본 공격 속성 (물리/마법/고정) - Skul이 정함

	Vector2 moveDirection = Vector2::Zero;
	float distance = 0.0f;           // 이번 공격의 목표 이동 거리
	float moveSpeed = 0.0f;          // distance / duration
	float moveTimer = 0.0f;          // 남은 이동 시간
	float traveled = 0.0f;           // 이번 공격에서 이동한 거리 (초과 방지용)

	float comboTimer = 0.0f;
	float comboInputTime = 0.4f;
};
