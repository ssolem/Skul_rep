#include "Framework.h"
#include "NinjaMeleeAttack.h"

NinjaMeleeAttack::NinjaMeleeAttack(Transform* target)
	: NinjaAttack("Ninja_Attack.xml", target)
{
	// 클립에 종결 이벤트 "끝" 할당
	clip->SetEvent(bind(&NinjaAttack::End, this));

	power = 100;

	// 충돌체 지정
	attackCollider = new RectCollider(Vector2(100, 100));
	attackCollider->Pos().x = 50.0f;
	attackCollider->SetParent(target);
				// target을 캐릭터로 지정 -> 충돌 표적은 충돌로 따로 판별하는 걸로
	attackCollider->SetActive(false); // 일단은 안 보임

	// 공격 중에 프레임별 이벤트 설정
	clip->SetEvent(bind(&NinjaAttack::EnableCollider, this), 4);
	clip->SetEvent(bind(&NinjaAttack::DisableCollider, this), 6);
}
