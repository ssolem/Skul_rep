#include "Framework.h"
#include "Tank.h"
#include "TankPatrol.h"
#include "TankTrace.h"
//#include "TankAttack.h"

Tank::Tank(AStar* aStar)
	:Quad(L"Textures/Shooting/Tank_Body.png"), aStar(aStar)
{
	//상태 최초 생성
	CreateStates();
	//SetState((int)State::PATROL);

	collider = new RectCollider(texture->GetSize());
	collider->SetTag("Tank Collider");
	collider->SetParent(this);

	turret = new Quad(L"Textures/Shooting/Tank_Head.png");
	turret->SetParent(this);

	//외부에서 탱크나 외부 인자(플레이어)에 의지하지 않고
	//필요하면 즉시 함수가 실행되도록 이벤트에 등록

	Observer::Get()->AddIntParamEvent("TankState", bind(&Tank::SetState, this, placeholders::_1));

}

Tank::~Tank()
{
	delete turret;
	delete collider;

	for (pair<State, TankState*> state : states)
	{
		delete state.second;
	}
}

void Tank::Update()
{
	states[curState]->Update();
	UpdateWorld();
	collider->UpdateWorld();
	turret->UpdateWorld();
}

void Tank::Render()
{
	Quad::Render();
	turret->Render();
	collider->Render();
	states[curState]->Render(); //디버그용 , 옵션
}

void Tank::PostRender()
{
	collider->RenderUI();
}

void Tank::SetState(int state)
{
	if (curState == (Tank::State)state) return;

	curState = (Tank::State)state;
	states[curState]->Start();

	//트랜스폼의 상태 코드에도 현재의 개별 클래스 상태를 투사
	//stateCode = state; 이렇게 쓰면 오류
	stateCode.store(state);
}

void Tank::CreateStates()
{
	//탱크에게 상태 별로 할일 주기
	
	TankPatrol* patrol = new TankPatrol(this);
	patrol->SetAStar(aStar);

	TankTrace* trace = new TankTrace(this);
	trace->SetAStar(aStar);

	//TankAttack* attack = new TankAttack(this);
	//attack->SetAStar(aStar);

	//탱크의 상태 맵 실제 행동 전달
	states[State::PATROL] = patrol;
	states[State::TRACE] = trace;
	//states[State::ATTACK] = attack;
}
