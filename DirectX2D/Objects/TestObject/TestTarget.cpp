#include "Framework.h"
#include "TestTarget.h"

TestTarget::TestTarget() : Quad(
	L"Textures/Shooting/rectEnemy.png")
{
	// 쿼드로서 필요한 것들은 자동으로 만들어져 있다

	// -> 여기서는 세팅 전에 충돌체를 준비해보자 (테스트 쿼드와 반대)
	collider = new RectCollider(texture->GetSize());
	collider->SetParent(this);

	// 그 다음에 본체를 준비
	Pos() = { CENTER_X, CENTER_Y };
}

TestTarget::~TestTarget()
{
	delete collider;
}

void TestTarget::Update()
{
	UpdateWorld();
	collider->UpdateWorld();
}

void TestTarget::Render()
{
	Quad::Render();
	collider->Render();
}

bool TestTarget::IsHit(Collider* collider)
{
	// 충돌 자체는 간단히 만들어진 기능을 부르기만 하면 된다
	bool result = this->collider->IsCollision(collider);

	// (옵션) AABB 써보고 싶다면
	// Vector2 overlap;
	// bool result = this->collider->IsCollision(collider, &overlap);

	if (result)
	{
		// IsHit 안에서 결정하고 싶은 후속 결과가 있으면 여기서 정하면 된다
	}

	return result; // 마지막으로 결과를 반환하고 종료
}
