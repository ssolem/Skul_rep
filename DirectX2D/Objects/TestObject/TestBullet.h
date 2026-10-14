#pragma once
class TestBullet : public Quad
{
public:

	TestBullet();
	~TestBullet();

	void Update();
	void Render();

	// 총알에게 필요한 공개 기능 : 발사되기
	void Fire(Vector2 pos, Vector2 direction); // 어디서, 어디로 발사되나

	Collider* GetCollider() { return collider; }

private:
	// 총알에게 필요한 고유 기능 : 이동(날아가기)
	void Move();

	// 고유 데이터
	float speed = 100;
	Vector2 direction;

	// 충돌체
	Collider* collider;
};

