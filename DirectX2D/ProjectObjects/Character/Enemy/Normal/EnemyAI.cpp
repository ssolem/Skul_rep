#include "Framework.h"
#include "EnemyAI.h"
#include "EnemyAttack.h"
#include "Enemy.h"
#include "ProjectObjects/Stage.h"

//LineCollider* EnemyAI::sightLine = nullptr;
//int EnemyAI::sightLineUsers = 0;

EnemyAI::EnemyAI(Enemy* owner)
	: owner(owner)
{
	//if (sightLineUsers++ == 0)
		//sightLine = new LineCollider(Vector2(), Vector2());

	SetDetectRange(detectionRange);
}

EnemyAI::~EnemyAI()
{
	for (EnemyAttack* attack : attacks)
		delete attack;

	delete detectCollider;

	//if (--sightLineUsers == 0)
	//{
		//delete sightLine;
		//sightLine = nullptr;
	//}
}

void EnemyAI::SetDetectRange(float range)
{
	detectionRange = range;

	delete detectCollider;

	detectCollider = new CircleCollider(range);
	detectCollider->SetParent(owner);
	detectCollider->Pos() = owner->GetCollider()->Pos();	
	detectCollider->SetTag("DetectRange");
}

void EnemyAI::Update()
{
	if (target == nullptr)
		target = owner->GetStage()->GetPlayer();
	if (target == nullptr) return;

	detectCollider->UpdateWorld();

	for (EnemyAttack* attack : attacks)
		attack->UpdateCoolDown();

	stateTimer += DELTA;

	switch (state)
	{
	case State::PATROL: UpdatePatrol(); break;
	case State::CHASE:  UpdateChase();  break;
	case State::ATTACK: UpdateAttack(); break;
	}
}

void EnemyAI::Render()
{
	detectCollider->Render();
	//sightLine->Render();		//공유 선이라 마지막으로 검사한 적의 시야선이 보임

	for (EnemyAttack* attack : attacks)
		attack->Render();		//공격별 사거리 + 실행 중인 히트박스
}

void EnemyAI::CancelAttack()
{
	if (curAttack)
	{
		curAttack->Cancel();
		curAttack = nullptr;
	}

	//공격 중에 맞았으면 후딜부터 다시 - 순찰/추적 중이었으면 그대로
	if (state == State::ATTACK)
		stateTimer = 0.0f;
}

void EnemyAI::UpdatePatrol()
{
	if (IsInDetectRange()/* && HasLineOfSight()*/)
	{
		ChangeState(State::CHASE);
		return;
	}

	if (!canMove || patrolDistance <= 0.0f) return;	

	Wander();
}

void EnemyAI::UpdateChase()
{
	if (!IsInDetectRange())
	{
		ChangeState(State::PATROL);
		return;
	}

	////모서리에 잠깐 가린 건 무시
	//if (HasLineOfSight())
	//	lostSightTimer = 0.0f;
	//else if ((lostSightTimer += DELTA) >= LOSE_SIGHT_TIME)
	//{
	//	ChangeState(State::PATROL);
	//	return;
	//}

	if (EnemyAttack* attack = PickAttack())
	{
		owner->FaceTo(target);
		curAttack = attack;
		curAttack->Start(target);
		ChangeState(State::ATTACK);
		return;
	}

	float diffX = target->GlobalPos().x - owner->GlobalPos().x;
	float targetDir = diffX > 0.0f ? 1.0f : -1.0f;

	if (!canMove) return;

	if(abs(diffX) <= STOP_DISTANCE)
	{
		owner->FaceTo(target);
		return;
	}

	if(isBlocked && targetDir != blockedDirX)
	{
		isBlocked = false;
	}

	if(isBlocked)
	{
		if(Wander())
			isBlocked = false;
		else if(stateTimer < 0.0f) //대기중
			owner->FaceTo(target);
		return;
	}

	owner->MoveToward(targetDir);

	if (owner->GetDirection().x == 0.0f)
	{
		isBlocked = true;
		blockedDirX = targetDir;
		isPatrolMoving = false;
		isWandering = false;
		stateTimer = 0.0f;
	}
}

//공격 실행 -> 끝나면 같은 상태에서 후딜 대기 -> CHASE
void EnemyAI::UpdateAttack()
{
	if (curAttack)
	{
		curAttack->Update();
		if (!curAttack->IsFinished()) return;

		curAttack = nullptr;
		stateTimer = 0.0f;		//여기부터 후딜 시간 계산
		return;
	}

	if (stateTimer >= recoverTime)
		ChangeState(State::CHASE);
}

void EnemyAI::ChangeState(State next)
{
	state = next;
	stateTimer = 0.0f;
	//lostSightTimer = 0.0f;
	isPatrolMoving = false;		//순찰로 돌아오면 새 랜덤 이동부터
	isWandering = false;
	isBlocked = false;

	//추적 중(PATROL 외)에는 감지 원을 키워서 경계에서 들락날락하지 않게
	float ratio = (next == State::PATROL) ? 1.0f : loseRangeRatio;
	detectCollider->Scale() = { ratio, ratio };
	detectCollider->UpdateWorld();
}

bool EnemyAI::Wander()
{	
	//대기 중 	stateTimer = -patrolWaitTime;
	if (stateTimer < 0.0f) return false;

	if(isWandering)
	{
		isWandering = false;
		return true;
	}

	//새 이동 시작 - 방향과 거리를 랜덤으로
	if (!isPatrolMoving)
	{
		patrolDir = Random(0.0f, 1.0f) < 0.5f ? -1.0f : 1.0f;
		patrolMoveDistance = Random(patrolminDistance, patrolDistance);
		patrolStartX = owner->GlobalPos().x;
		isPatrolMoving = true;
	}

	float moved = abs(owner->GlobalPos().x - patrolStartX);
	if (moved < patrolMoveDistance)
	{
		owner->MoveToward(patrolDir);

		//벽/낭떠러지에 막히면 MoveToward가 dir을 0으로 만듦 -> 이번 이동 종료
		if (owner->GetDirection().x != 0.0f) return false;
	}

	//목표 거리 도달 또는 GetDirection().x == 0.0f, 대기 후 다시 랜덤 이동
	isPatrolMoving = false;
	isWandering = true;
	stateTimer = -patrolWaitTime;		//음수에서 0까지 = 대기 시간
	return false;
}

bool EnemyAI::IsInDetectRange()
{
	if (!target->Active()) return false;

	return detectCollider->IsCollision(target->GetCollider());
}

//몸 중심끼리 선을 그어 벽/발판 타일에 걸리면 안 보이는 것
//bool EnemyAI::HasLineOfSight()
//{
//	sightLine->SetPoints(owner->GetCollider()->GlobalPos(), target->GetCollider()->GlobalPos());
//
//	Stage* stage = owner->GetStage();
//	return stage->CheckWallTileCollision(sightLine) == nullptr
//		&& stage->CheckFloorTileCollision(sightLine) == nullptr;
//}

//우선순위 후보들 추려서 가중치 랜덤
EnemyAttack* EnemyAI::PickAttack()
{
	vector<EnemyAttack*> candidates;
	int topPriority = INT_MIN;
	float totalWeight = 0.0f;

	for (EnemyAttack* attack : attacks)
	{
		if (!attack->CanAttack(target)) continue;

		if (attack->GetPriority() > topPriority)
		{
			topPriority = attack->GetPriority();
			candidates.clear();
			totalWeight = 0.0f;
		}

		if (attack->GetPriority() == topPriority)
		{
			candidates.push_back(attack);
			totalWeight += attack->GetWeight();
		}
	}

	if (candidates.empty()) return nullptr;

	float pick = Random(0.0f, totalWeight);
	for (EnemyAttack* attack : candidates)
	{
		pick -= attack->GetWeight();
		if (pick <= 0.0f) return attack;
	}
	return candidates.back();
}
