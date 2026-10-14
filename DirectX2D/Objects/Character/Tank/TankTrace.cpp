#include "Framework.h"
#include "TankTrace.h"
#include "Tank.h"

TankTrace::TankTrace(Transform* tank)
	: TankState(tank), checkTarget(&TankTrace::CheckTarget, this)
{
	rangeCollider = new CircleCollider(ATTACK_RANGE * 2);
	rangeCollider->SetParent(tank);
	rangeCollider->GetColor() = { 1, 0, 0, 1 };

	// 
	Observer::Get()->AddParamEvent("TankTraceSetTarget", bind(&TankTrace::SetTarget, this, placeholders::_1));

	//checkTarget.detach();
}

TankTrace::~TankTrace()
{
	if (checkTarget.joinable())
		checkTarget.join();

	delete rangeCollider;
}

void TankTrace::Update()
{
	traceRetension += DELTA;
	if (traceRetension > RETENSION_TIME)
	{
		traceRetension -= RETENSION_TIME;
		Trace();
	}

	Move(); //계속 추적 상태이고, 길이 만들어져 있으면 움직이기

	rangeCollider->UpdateWorld();
}

void TankTrace::Render()
{
	rangeCollider->Render();
}

void TankTrace::Start()
{
	traceRetension = 0.0f;
}

void TankTrace::SetDestPos(Vector2 pos)
{
}

void TankTrace::SetTarget(void* target)
{
	Transform* targetTransform = (Transform*)target;
	this->target = targetTransform;
}

void TankTrace::Move()
{
	if (path.empty())
		return;

	Vector2 target = path.back();
	float distance = Distance(tank->Pos(), target);

	float tickDistance = speed * DELTA;

	if (distance <= tickDistance)
	{
		tank->Pos() = target;
		path.pop_back();
		return;
	}

	Vector2 direction = (target - tank->Pos()).GetNormalized();
	tank->Rot().z = direction.Angle();

	tank->Pos() += direction * tickDistance;
}

void TankTrace::Trace()
{
	int start = aStar->FindClosestNode(tank->Pos());
	int end = aStar->FindClosestNode(target->Pos());

	aStar->GetPath(start, end, path);
}

void TankTrace::CheckTarget()
{
	float time = 0;

	while (true)
	{
		time += DELTA;
		if (time < 0.1f)
			continue;

		time -= 0.1f;

		//checkMutex.lock();
		int state = tank->stateCode.load();
		if (state == (int)Tank::State::TRACE)
		{
			float distance = Distance(tank->Pos(), target->Pos());

			if (distance < ATTACK_RANGE) //우선순위 생각하기
			{
				//Observer::Get()->ExcuteIntParamEvent("TankState", (int)Tank::State::ATTACK);
			}
			//else if (distance < PATROL_RANGE)
			else if (distance > PATROL_RANGE)
			{
				Observer::Get()->ExecuteIntParamEvent("TankState", (int)Tank::State::PATROL);
			}
		}

		//checkMutex.unlock();
	}
}