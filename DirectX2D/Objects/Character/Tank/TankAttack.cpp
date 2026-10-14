#include "Framework.h"
#include "TankAttack.h"
#include "Tank.h"

TankAttack::TankAttack(Transform* tank)
	: TankState(tank), checkTarget(&TankAttack::CheckTarget, this)
{
	rangeCollider = new CircleCollider(RANGE * 2);
	rangeCollider->SetParent(tank);
 
	Observer::Get()->AddParamEvent("TankAttackSetTarget", bind(&TankAttack::SetTarget, this, placeholders::_1));

	checkTarget.detach();
}

TankAttack::~TankAttack()
{
	if (checkTarget.joinable())
		checkTarget.join();

	delete rangeCollider;
}

void TankAttack::Update()
{
	Attack();
	rangeCollider->UpdateWorld();
}

void TankAttack::Render()
{
	rangeCollider->Render();
}

void TankAttack::Start()
{
	attackInterval = INTERVAL_TIME;
}

void TankAttack::Attack()
{
	attackInterval += DELTA;

	if (attackInterval < INTERVAL_TIME)
		return;

	attackInterval -= INTERVAL_TIME;

	Vector2 dir = target->Pos() - tank->Pos();
	Fire(dir);
}

void TankAttack::Fire(Vector2 dir)
{
	Quad* turret = ((Tank*)tank)->GetTurret();
	turret->Rot().z = dir.Angle();
	BulletManager::Get()->Fire(turret->GlobalPos(), turret->Right(), 0);
}

void TankAttack::SetTarget(void* target)
{
	Transform* targetTransform = (Transform*)target;
	this->target = targetTransform;
}

void TankAttack::CheckTarget()
{
	float time = 0;

	while (true)
	{
		time += DELTA;
		if (time < 0.1f)
			continue;

		time -= 0.1f;

		float distance = Distance(tank->Pos(), target->Pos());

		checkMutex.lock();

		if (distance > RANGE)
		{
			Observer::Get()->ExecuteIntParamEvent("TankState", (int)Tank::State::TRACE);
		}

		checkMutex.unlock();
	}
}
