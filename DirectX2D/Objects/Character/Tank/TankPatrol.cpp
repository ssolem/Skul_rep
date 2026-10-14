#include "Framework.h"
#include "TankPatrol.h"
#include "Tank.h"

TankPatrol::TankPatrol(Transform* tank)
	: TankState(tank)
	, checkTarget(&TankPatrol::CheckTarget, this) //스레드 등록 -> 돌아갈 준비

{
	rangeCollider = new CircleCollider(RANGE * 2);
	rangeCollider->SetParent(tank);

	// 순찰 중에 위의 범위에 뭔가 잡히거나 놓치면 갱신하기 위한 이벤트
	Observer::Get()->AddParamEvent("TankPatrolSetTarget", bind(&TankPatrol::SetTarget, this, placeholders::_1));

	// 스레드의 사용
	// 0. 기본적으로 스레드는 할당이 되면 즉시 돌아가는게 시작된다.
	//		그리고 스레드에 등록된 함수의 실행은 다른 함수, 다른 절차와는 별개
	//		연산 단위로 실행이 대기 되어 매번 CPU가 연산을 병렬 처리

	// 1. 다만 여기서, 스레드가 아직 완전히 CPU의 연산 대기열에서 자유롭지는 않다.
	//	텍스트 차원의 대기열이 다중 대기열로 바뀐 것 (줄은 2줄인데 출입구는 하나)

	// 2. 여기서 스레드를 완전히 앱에서 분리시켜서 실행의 시작 끝의 호출을( 실행은 CPU에 달림 )
	//    독립시키는 것이
	//checkTarget.detach();

	// 분리를 사용하면 이 때부터 여기 관여하는 최소한의 변수 외에, 그 어떤 연산의 상호작용도 없다.

	// 단 분리를 이용하면, 앱의 연산으로부터 완전 독립되기 때문에
	// 앱을 끝내는 순간까지도 해당 함수는 계쏙 돌아간다. -> 오류의 온상

	//detach 를 사용한 함수는 합류 가능한 상태인지 확인 -> 합류 호출을 해서 분리를 끝내야만 한다.
	//if (checkTarget.joinable())
	//	checkTarget.join();
}

TankPatrol::~TankPatrol()
{
	if (checkTarget.joinable())
		checkTarget.join(); // 스레드 합치기 (분리 종료, 개별 연산 멈추기)
	delete rangeCollider;
}

void TankPatrol::Update()
{
	if (inPatrol)
		Patrol();
	else
	{
		stopTime += DELTA;

		if (stopTime > STOP_TIME)
			Start();
	}

	//표적 확인은 스레드를 관리하는 다른 곳에서 별도 실행 - detach

	rangeCollider->UpdateWorld();
}

void TankPatrol::Render()
{
	rangeCollider->Render();
}

void TankPatrol::Start()
{
	inPatrol = true;
	stopTime = 0;

	//A*로 길찾기
	int start = aStar->FindClosestNode(tank->Pos());

	//목적지를 랜덤으로 만들기
	//float x = Random(-RANGE, RANGE);
	//float y = Random(-RANGE, RANGE);
	//Vector2 temp = tank->Pos() + Vector2(x, y);
	//int end = aStar->FindClosestNode(temp);

	int end = aStar->FindRandomNodeInRange(tank->Pos(), RANGE);

	if (start < 0 || end < 0 || start == end)
	{
		inPatrol = false;
		return;
	}

	aStar->GetPath(start, end, path); //길 찾기
	inPatrol = !path.empty();

}

void TankPatrol::SetTarget(void* target)
{
	Transform* targetTransform = (Transform*)target;
	this->target = targetTransform;

	//개별 표적이 고유 기능 등이 있으면 이 아래, 혹은 다른 기능을 통해서 작성
}

void TankPatrol::Patrol()
{
	//순찰 : 일단 길 따라 가기
	if (path.empty())
	{
		inPatrol = false; // 갈 길 없으면 이번 순찰 종료(재시작 혹은 다른 행동하기)
		return;
	}

	Vector2 target = path.back();
	float distance = Distance(tank->Pos(), target);

	float tickDistance = speed * DELTA;

	if (distance <= tickDistance)
	{
		tank->Pos() = target;
		path.pop_back();
		return;
	}

	//여기로 왔다면 남은 거리가 있는 상황
	Vector2 direction = (target - tank->Pos()).GetNormalized();
	tank->Rot().z = direction.Angle();

	//Pos() += direction * MOVE_SPEED * DELTA;
	tank->Pos() += direction * tickDistance;
}

void TankPatrol::CheckTarget()
{
	//표적이 있거나, 공격(추적) 거리안에 있거나 등을 살펴보는 확인 연산
	//따로 연산을 함과 동시에 thread화
	// 여기에서 다른 상태와 동시에 수행되지 못하게 강제 = 뮤텍스

	float time = 0;

	while (true)
	{
		time += DELTA;
		if (time < 0.1f) // 행동 변화를 위한 최소 진행 시간
			continue;

		time -= 0.1f; // 진행 시간마다 별도의 틱 계산 (윈도우 앱 업데이트 아님)

		// 이하 연산(순찰 중 확인)을 진행하는 동안 다른 상태 계산이 불가능해지게 한다.

		//checkMutex.lock(); //뮤텍스를 잠궈서 다른 행동이 불가능해지게

		int state = tank->stateCode.load();
		if ( state == (int)Tank::State::PATROL)
		{
			float distance = Distance(tank->Pos(), target->Pos());

			if (distance < RANGE) // 감지 거리 안
			{
				// 이후 같은 방식으로 '탱크 추적' 이벤트를 준비, 등록해뒀다가
				Observer::Get()->ExecuteIntParamEvent("TankState", (int)Tank::State::TRACE);
			}
		}


		//연산이 끝나면 다시 풀어서 (다른 상태 진행 중이면) 연산을 이어받을 수 있게

		//checkMutex.unlock();
	}
}
