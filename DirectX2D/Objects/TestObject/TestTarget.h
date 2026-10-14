#pragma once

class TestTarget : public Quad
{
public:

	TestTarget();
	~TestTarget();

	void Update();
	void Render();

	bool IsHit(Collider* collider); // <- 충돌 판별이면서, 충돌 처리도 하도록 하는 함수
	Collider* GetCollider() { return collider; } // 충돌체 정보를 노출하는 함수

private:

	Collider* collider;

};

