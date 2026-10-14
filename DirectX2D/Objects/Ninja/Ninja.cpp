#include "Framework.h"
#include "Ninja.h"

// 액션 포함
#include "NinjaIdle.h"
#include "NinjaRun.h"
#include "NinjaJump.h"
#include "NinjaJumpUp.h"
#include "NinjaJumpDown.h"

#include "NinjaMeleeAttack.h"
#include "NinjaJumpAttack.h"

Ninja::Ninja()
{
	// 클립이 없으므로, 클립을 위한 데이터는 모두 소거
	// 클립, 트랜스폼 데이터 등등

	SetPixelShader(L"Ninja.hlsl");

	Pos() = { CENTER_X, CENTER_Y };

	CreateActions();
	SetAction(ActionType::IDLE); // 일단은 가만히 있는 걸로 시작


	Observer::Get()->AddIntParamEvent(
		"Action",	// 커맨드 실행을 부르기 위한 호출 키
		bind(&Ninja::SetActionCode, this, placeholders::_1)
	);

	secondMap = Texture::Add(L"Textures/Effect/rainbow.png");

	intValueBuffer = new IntValueBuffer();
	floatValueBuffer = new FloatValueBuffer();
	
	collider = new RectCollider(actions[curAction]->GetClip()->GetCurFrame()->GetSize());
	collider->SetParent(this);
}

Ninja::~Ninja()
{
	for (pair<ActionType, Action*> action : actions)
		delete action.second;

	delete intValueBuffer;
	delete floatValueBuffer;
}

void Ninja::Update()
{
	ControlTemp();

	actions[curAction]->Update(); // 현재 액션의 클립 업데이트
	UpdateWorld(); // 게임오브젝트로서
	collider->UpdateWorld();

	//지금 있는 그림을 통해 외곽선의 크기 기준 구하기
	Vector2 size = actions[curAction]->GetClip()->GetCurFrame()->GetSize();
	floatValueBuffer->Get()[0] = size.x;
	floatValueBuffer->Get()[1] = size.y;
}

void Ninja::Render()
{
	//외곽선용 데이터 등록
	secondMap->PSSet(1);

	intValueBuffer->SetPS(1);
	floatValueBuffer->SetPS(2);

	SetRender();

	actions[curAction]->Render();
	collider->Render();
}

void Ninja::RenderUI()
{
	ImGui::SliderInt("Weight", intValueBuffer->Get(), 0, 10);
	ImGui::SliderInt("Scale", &intValueBuffer->Get()[1], 0, 10);
}

void Ninja::ControlTemp()
{
	Move();
	Jump();
	Attack();
}

void Ninja::Move()
{
	// 무시조건 설정
	if (curAction >= ActionType::JUMP &&
		curAction <= ActionType::JUMP_DOWN)	// 점프로 분류되는 행동 중이면
		return;

	if (curAction >= ActionType::MELEE_ATTACK) return; //공격중 이동 없음

	// -> 실행

	bool isMove = false;

	isMove |= KEY_PRESS(VK_RIGHT); // 둘 중 하나가 참이면 참
	isMove |= KEY_PRESS(VK_LEFT);

	isMove ? SetAction(ActionType::RUN)
		: SetAction(ActionType::IDLE);

	// 달리는 중이면, 달리는 액션에 속력을 부여
	if (curAction == ActionType::RUN)
	{
		float velocity = KEY_PRESS(VK_RIGHT) ? 1 : -1;

		NinjaRun* run = (NinjaRun*)actions[curAction];
		run->SetVelocity(velocity);
	}
}

void Ninja::Jump()
{
	// 업데이트 필수사항 먼저
	// 점프 중 가로 제어
	if (curAction >= ActionType::JUMP &&
		curAction <= ActionType::JUMP_DOWN)
	{
		float horisontal = 0;
		if (KEY_PRESS(VK_RIGHT)) horisontal = 1;
		else if (KEY_PRESS(VK_LEFT)) horisontal = -1;

		NinjaJump* jump = (NinjaJump*)actions[curAction];
		jump->SetHorisontal(horisontal);
	}

	// 무시조건
	if (curAction >= ActionType::MELEE_ATTACK) return;

	if (curAction >= ActionType::JUMP &&
		curAction <= ActionType::JUMP_DOWN)	// 이미 점프 중이라면
		return;

	if (KEY_DOWN(VK_SPACE))
		SetAction(ActionType::JUMP_UP); // "상승" 시작
}

void Ninja::Attack()
{
	if (curAction >= ActionType::MELEE_ATTACK) return;

	if (KEY_DOWN('A'))
	{
		if (curAction >= ActionType::JUMP &&
			curAction <= ActionType::JUMP_DOWN)
		{
			SetAction(ActionType::JUMP_ATTACK);
		}
		else
			SetAction(ActionType::MELEE_ATTACK);
	}
}

void Ninja::SetAction(ActionType type)
{
	if (curAction == type) return;

	curAction = type;		// 상태 전환 후
	actions[type]->Start(); // 해당 상태의 클립을 재생
}

void Ninja::SetActionCode(int type)
{
	SetAction((ActionType)type);
}

void Ninja::SetIdle()
{
	SetAction(ActionType::IDLE);
}

void Ninja::SetFall()
{
	SetAction(ActionType::JUMP_DOWN);
}

void Ninja::CreateActions()
{
	actions[ActionType::IDLE] = new NinjaIdle();
	
	actions[ActionType::MELEE_ATTACK] =
		new NinjaMeleeAttack(this);
	actions[ActionType::MELEE_ATTACK]->SetEvent(
		bind(&Ninja::SetIdle, this));

	actions[ActionType::JUMP] = new NinjaJump(this);
	actions[ActionType::JUMP]->SetEvent(
		bind(&Ninja::SetIdle, this));

	actions[ActionType::RUN] = new NinjaRun(this);

	actions[ActionType::JUMP_UP] = new NinjaJumpUp(this);
	actions[ActionType::JUMP_UP]->SetEvent(
		bind(&Ninja::SetFall, this));
	
	actions[ActionType::JUMP_DOWN] = new NinjaJumpDown(this);
	actions[ActionType::JUMP_DOWN]->SetEvent(
		bind(&Ninja::SetIdle, this));

	actions[ActionType::JUMP_ATTACK] = new NinjaJumpAttack(this);
	actions[ActionType::JUMP_ATTACK]->SetEvent(
		bind(&Ninja::SetFall, this));
}
