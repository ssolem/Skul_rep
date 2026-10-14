#include "Framework.h"
#include "NinjaRun.h"

NinjaRun::NinjaRun(Transform* target)
	: Action ("Textures/Ninja/",
		"Ninja_Run.xml", true),
		target(target)
{
}

void NinjaRun::Update()
{
	target->Pos().x += velocity * MOVE_SPEED * DELTA;

	// 속도에 따라 그림 전환 -> 닌자가 바뀌어야 한다

	//float rotOffset = XM_PI * 0.5f;
	//target->Rot().y = velocity * -rotOffset + rotOffset;  // 1 : 0, -1 : XM_PI

	// ...위의 식이 Dx 상에서의 바른 접근이고, 귀찮다면 아래처럼

	target->Scale().x = velocity; // 크기가 1이냐. -1이냐 (winAPI의 stretchBlt)

	Action::Update();
}
