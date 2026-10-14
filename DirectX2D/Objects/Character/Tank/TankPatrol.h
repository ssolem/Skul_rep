#pragma once
#include "TankState.h"

class TankPatrol : public TankState
{
	// 상태에 따른 행동(Task) : 순찰

private:
	const float RANGE = 200.0f;
	const float STOP_TIME = 1.0f;

public:
	TankPatrol(Transform* tank);
	~TankPatrol();

	virtual void Update() override;
	virtual void Render() override;
	virtual void Start() override;

	void SetAStar(AStar* aStar) { this->aStar = aStar; }
	void SetTarget(void* target);

private:
	void Patrol();
	void CheckTarget();

private:
	CircleCollider* rangeCollider;
	AStar* aStar;

	bool inPatrol = false;
	float stopTime = 0;

	vector<Vector2> path;
	Transform* target = nullptr;

	//-> 순찰을 하다가 표적이 있는지 찾기
	//   순찰을 하면서 추적은 안하기
	//   순찰을 하다가 표적을 찾으면 추적으로 넘어가기
	//   순찰을 하다가 추적을 한다면 -> 순찰을 안하기

	thread checkTarget; //스레드를 통한 동시 진행
	mutex checkMutex;  //상호 배제 (Mutual Exclusion) -공유 자원에 접근할 때 문제가 발생하지 않도록 단 하나의 자원 점유만 허용
};

