#include "Framework.h"
#include "NinjaJump.h"

NinjaJump::NinjaJump(Transform* target) :
	Action("Textures/Ninja/", "Ninja_Jump.xml", false),
	target(target)
{
}

void NinjaJump::Update()
{
	// 점프 도중에 계속 적용되는 계산

	velocity -= GRAVITY * DELTA; // 속력에서 중력만큼을 계속 빼기

	target->Pos().y += velocity * DELTA; // 빠진 만큼을 위치에 반영

	// -> velocity가 +면 상승, -는 하강
	//    시간이 지날수록 상승 속도 감소 -> 공중 정지 ->
	//    하강 시작 -> 하강 속도 상승 순으로 진행이 될 것

	// ---------------------------------------------------------
	// 가로 이동 부분

	target->Pos().x += horisontal * MOVE_SPEED * DELTA;
	if (horisontal != 0)
		target->Scale().x = horisontal;

	// ---------------------------------------------------------

	//if (target->Pos().y < CENTER_Y)
	//{
	//	target->Pos().y = CENTER_Y; // 발이 바닥 밑으로 꺼지지 않게
	//	velocity = 0;

	//	event(); // 마지막 이벤트 (다른 상태로 전이 예약)
	//}

	Action::Update(); // 클립 업데이트
}

void NinjaJump::Start()
{
	Action::Start();
	velocity = JUMP_POWER; // 점프력 부여
}
