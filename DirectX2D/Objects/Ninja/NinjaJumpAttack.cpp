#include "Framework.h"
#include "NinjaJumpAttack.h"

NinjaJumpAttack::NinjaJumpAttack(Transform* target)
	: NinjaAttack("Ninja_JumpAttack.xml", target)
{
	// 종료 이벤트를 자체 지정하지 않고, 점프에 대해 어떻게 할 것인지 사후호출

	power = 100;

	// 충돌체 지정
	attackCollider = new RectCollider(Vector2(70, 140));
	attackCollider->Pos().x = 50.0f;
	attackCollider->SetParent(target);
	attackCollider->SetActive(false);

	clip->SetEvent(bind(&NinjaAttack::EnableCollider, this), 4);
	clip->SetEvent(bind(&NinjaAttack::DisableCollider, this), 7);
	// -> 동작이 다 끝나는 것까지 공격에 포함 (동작 끝나면 후속동작)

	clip->SetEvent(bind(&NinjaAttack::End, this));
}

void NinjaJumpAttack::Update()
{
	Action::Update(); // 클립 이벤트 감시 실행용
}
