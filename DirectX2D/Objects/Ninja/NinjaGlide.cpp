#include "Framework.h"
#include "NinjaGlide.h"

NinjaGlide::NinjaGlide(Transform* target)
	: NinjaJump(target)
{
	Call_LoadClip("Textures/Ninja/", "Ninja_Glide.xml", false, 1);

	velocity = -10; // 속도 직접 
}

void NinjaGlide::Update()
{
	target->Pos().y += velocity * DELTA; // 낮은 속도로 일정하게 내려오기

	// 가로이동이 필요하면
	target->Pos().x += horisontal * MOVE_SPEED * DELTA;
	target->Scale().x = horisontal;

	Action::Update();
}
