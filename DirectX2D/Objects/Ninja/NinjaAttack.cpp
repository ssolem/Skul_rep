#include "Framework.h"
#include "NinjaAttack.h"

NinjaAttack::NinjaAttack(string file, Transform* target)
	: Action("Textures/Ninja/", file, false), target(target)
{
	// 공격 충돌체는 각자 세부 공격에서 설정
}

NinjaAttack::~NinjaAttack()
{
	delete attackCollider;
}

void NinjaAttack::Render()
{
	Action::Render();
	attackCollider->Render();
}

void NinjaAttack::End()
{
	// 받은 이벤트 (미지정 == 종료 후) 있으면 실행
	event();
}

void NinjaAttack::EnableCollider()
{
	attackCollider->UpdateWorld();
	attackCollider->SetActive(true);
}

void NinjaAttack::DisableCollider()
{
	attackCollider->SetActive(false);
}
