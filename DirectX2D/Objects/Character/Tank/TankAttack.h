#pragma once
#include "TankState.h"


class TankAttack : public TankState
{
	// 상태에 따른 행동(Task) : 추적
private:
	const float RANGE = 150.0f;

	const float INTERVAL_TIME = 1.0f; //공격 간격
	const float RETENSION_TIME = 0.5f;

public:
	TankAttack(Transform* tank);
	~TankAttack();

	virtual void Update() override;
	virtual void Render() override;
	virtual void Start() override;

	//void SetAStar(AStar* aStar) { this->aStar = aStar; }
	void SetTarget(void* target);

private:
	void Attack();
	void Fire(Vector2 dir);
	void CheckTarget();

private:
	CircleCollider* rangeCollider;
	//AStar* aStar;

	//vector<Vector2> path;
	Transform* target = nullptr;

	float attackInterval = 0; //공격 간격 업데이트

	thread checkTarget; //스레드를 통한 동시 진행
	mutex checkMutex;  //상호 배제 (Mutual Exclusion) 
};

