#pragma once
class Enemy;
class Character;
class EnemyAttack;

//판단만 담당
class EnemyAI
{
public:
	enum class State
	{
		PATROL,		
		CHASE,
		ATTACK,		
	};

	EnemyAI(Enemy* owner);
	~EnemyAI();

	void Update();			
	void Render();			
	void CancelAttack();   
	void UpdateAttackEffects();	//curAttack 여부와 관계없이 모든 공격의 이펙트 갱신

	void AddAttack(EnemyAttack* attack) { attacks.push_back(attack); }
	void SetDetectRange(float range);
	void SetPatrolDistance(float distance) { patrolDistance = distance; }	//한 번에 이동할 최대 거리 (0이면 제자리)
	void SetPatrolWaitTime(float time) { patrolWaitTime = time; }
	void SetCanMove(bool canMove) { this->canMove = canMove; }	//false: 포탑형
	void SetRecoverTime(float time) { recoverTime = time; }

	bool GetCanMove() { return canMove; }

private:
	void UpdatePatrol();
	void UpdateChase();
	void UpdateAttack();
	void ChangeState(State next);
	bool Wander();

	bool IsInDetectRange();
	//bool HasLineOfSight();
	EnemyAttack* PickAttack();

private:
	const float STOP_DISTANCE = 5.0f;		//이 이하로 붙으면 제자리
	//const float LOSE_SIGHT_TIME = 1.0f;		//추적 중 시야가 이만큼 계속 가려지면 포기

	////공유 LineCollider로 벽 / 발판 타일에 시야가 막혔는지 확인
	//static LineCollider* sightLine;
	//static int sightLineUsers;

	Enemy* owner;
	Character* target = nullptr;
	State state = State::PATROL;
	float stateTimer = 0.0f;
	//float lostSightTimer = 0.0f;


	std::vector<EnemyAttack*> attacks;
	EnemyAttack* curAttack = nullptr;

	float detectionRange = 300.0f;
	CircleCollider* detectCollider = nullptr;
	float loseRangeRatio = 1.1f;		
	float recoverTime = 0.8f;			

	bool isPatrolMoving = false;
	bool isWandering = false;
	float patrolStartX = 0.0f;			
	float patrolMoveDistance = 0.0f;	
	float patrolminDistance = 40.0f;		
	float patrolDistance = 150.0f;
	float patrolWaitTime = 1.0f;
	float patrolDir = 1.0f;
	bool canMove = true;

	bool isBlocked = false;
	float blockedDirX= 0.0f;
};
