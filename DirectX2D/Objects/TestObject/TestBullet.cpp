#include "Framework.h"
#include "TestBullet.h"

TestBullet::TestBullet()
	:Quad(L"Textures/Shooting/playerBullet2.png")
{
	// 총알이 만들어지는 순간에는 아직 활성화 불가
	// 발사는 호출에 의해 이루어지게

	isActive = false; // 활성화 끄기

	//collider = new RectCollider(texture->GetSize());
	collider = new CircleCollider(25);
	collider->SetParent(this);
}

TestBullet::~TestBullet()
{
	delete collider;
}

void TestBullet::Update()
{
	// 활성화가 되어 있을 때만 진행되게
	if (!isActive) return;

	// 이동 수행
	Move();

	UpdateWorld();
	collider->UpdateWorld();
}

void TestBullet::Render()
{
	// 활성화가 되어 있을 때만 진행되게
	if (!isActive) return;

	Quad::Render();
	collider->Render();
}

void TestBullet::Fire(Vector2 pos, Vector2 direction)
{
	// 발사 시작 (활성화와 동시에 어디로 갈지를 지정받기)
	isActive = true;

	Pos() = pos;
	this->direction = direction;

	Rot().z = direction.Angle();
}

void TestBullet::Move()
{
	// 방향에 따라 직진
	Pos() += direction * speed * DELTA;

	if (Pos().x < 0 || Pos().x > WIN_WIDTH) isActive = false;
	if (Pos().y < 0 || Pos().y > WIN_HEIGHT) isActive = false;
}