#pragma once
#include "TankHeader.h"

class Tank : public Quad
{
public:
	enum class State
	{
		NONE, //상태 미설정
		PATROL, TRACE, ATTACK, //이동순찰, 추적, 공격
		HIT, DEAD, //피격, 퇴장
	};

public:
	Tank(AStar* aStar);
	~Tank();

	void Update();
	void Render();
	void PostRender();

	RectCollider* GetCollider() { return collider; }
	void SetTarget(Transform* target) { this->target = target; }
	Quad* GetTurret() { return turret; }

private:
	void SetState(int state);
	void CreateStates();	//상태에 따른 행동 추가

private:
	RectCollider* collider;

	Quad* turret;

	map<State, TankState*> states; //상태(그리고 상태별 행동)
	State curState = State::NONE;

	AStar* aStar;
	Transform* target;		//표적을 볼 수 있도록

};

