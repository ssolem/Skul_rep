#pragma once
#include "TankState.h"

class TankTrace : public TankState
{
	// 상태에 따른 행동(Task) : 추적
private:
	const float PATROL_RANGE = 250.f;
	const float ATTACK_RANGE = 150.0f;

	const float RETENSION_TIME = 0.5f;

public:
	TankTrace(Transform* tank);
	~TankTrace();

	virtual void Update() override;
	virtual void Render() override;
	virtual void Start() override;
	virtual void SetDestPos(Vector2 pos) override;

	void SetAStar(AStar* aStar) { this->aStar = aStar; }
	void SetTarget(void* target);

private:
	void Move();
	void Trace();
	void CheckTarget();

private:
	CircleCollider* rangeCollider;
	AStar* aStar;

	vector<Vector2> path;
	Transform* target = nullptr;

	float traceRetension = 0; //추적 시간 업데이트

	thread checkTarget; //스레드를 통한 동시 진행
	mutex checkMutex;  //상호 배제 (Mutual Exclusion) 
};

