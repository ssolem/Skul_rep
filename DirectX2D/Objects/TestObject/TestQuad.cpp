#include "Framework.h"
#include "TestQuad.h"

TestQuad::TestQuad() : Quad(L"Textures/Shooting/airplane.png")
{
	// 여기서 충돌체를 부르면 : 충돌체부터 준비하고 -> 이동하겠다는 뜻

	Pos() = { CENTER_X, CENTER_Y };
	ResetDirection();
	rotation = 0;

	// 여기서 충돌체를 부르면 : 본체를 제대로 세팅해두고 -> 부착물을 생성하겠다는 뜻

	collider = new RectCollider(texture->GetSize());
	// collider->Pos() = Pos(); // <- 이것도 동기화의 방법일 수 있는데
	collider->SetParent(this);  // * 테스트 개체를 충돌체의 부모로 설정

	// '부가 물품' 생성하기 좋은 타이밍 #1 : 객체는 만들었고 규칙 설정하기 직전

	turret = new Quad(L"Textures/Shooting/player.png");
	turret->SetParent(this);
	turret->Pos() += { 100, 0 };

	SetController();

	// '부가 물품' 생성하기 좋은 타이밍 #2 : 모든 부가 규칙이 끝난 뒤

	mags = new ProgressBar(L"Textures/UI/hp_bar.png", L"Textures/UI/hp_bar_BG.png");
	mags->SetParent(this);
	mags->Pos() += { 0, 150 };
	mags->Scale() *= 1.5f;
	mags->SetAmount(1.0f);

	maxAmmo = 100;
	curAmmo = maxAmmo;
}

TestQuad::~TestQuad()
{
	delete controller;
	delete collider;
	delete turret;
	delete mags;
}

void TestQuad::Update()
{
	controller->Update();

	Move();
	Rotate();
	FollowMouseCursor();

	UpdateWorld();
	turret->UpdateWorld();
	collider->UpdateWorld();
	mags->UpdateWorld();
}

void TestQuad::Render()
{
	Quad::Render();
	turret->Render();
	collider->Render(); // 자기 자신과 더불어 충돌체까지 모두 표시

	//mags->Render(); //2d에서는 나쁘지 않은 선택
}

void TestQuad::PostRender()
{
	mags->Render();
}

void TestQuad::Rotate()
{
	if (abs(rotation) < FLT_EPSILON) return;

	turret->Rot().z += rotation * rotationMult * DELTA;

	rotation = 0;
}

void TestQuad::Move()
{
	if (dir.Length() < FLT_EPSILON) return;

	dir.Normalize();
	Pos() += dir * speedMult * DELTA;

	ResetDirection();
}

void TestQuad::FollowMouseCursor()
{
	if (mouseMoveMode)
		Pos() = mousePos;
}

void TestQuad::RotateCW(void* value)
{
	float getValue = *(float*)value;
	rotation -= getValue;
}

void TestQuad::RotateCCW(void* value)
{
	float getValue = *(float*)value;
	rotation += getValue;
}

void TestQuad::MoveUp(void* value)
{
	float getValue = *(float*)value;
	dir += Vector2(0, +1 * getValue);
}

void TestQuad::MoveDown(void* value)
{
	float getValue = *(float*)value;
	dir += Vector2(0, -1 * getValue);
}

void TestQuad::MoveLeft(void* value)
{
	float getValue = *(float*)value;
	dir += Vector2(-1 * getValue, 0);
}

void TestQuad::MoveRight(void* value)
{
	float getValue = *(float*)value;
	dir += Vector2(+1 * getValue, 0);
}

void TestQuad::MouseMove(void* value)
{
	mouseMoveMode = *((bool*)value);
}

void TestQuad::MoveVector2(void* value)
{
	// 입력을 받아서, 비행기의 dir을 직접 설정하는 게 목적
	// 따라서 상하좌우 float 때와 크게 다를 게 없다
	Vector2 getValue = *(Vector2*)value;
	dir += getValue;
	// 이걸 활용하는 방법?
	// 1. 입력을 받을 때, 겟터를 구성해서 실행한다.... (복잡)
	// 2. 입력을 받을 때, 먼저 실행하는 쪽을 아래쪽 함수로 정하고
	//    거기서 나온 결과를 이 함수가 인수로 받도록 하면?
	//    다른 말로 "MoveVector2(x, y)" 자체가 하나의 결과입니다...라고 한다면???
	//    람다 식으로 만들어 대상화하자는 이야기
}
void TestQuad::MoveVector2(float x, float y)
{
	moveValue = { x, y };			// x, y를 받으면 여기서 벡터를 구성
	MoveVector2(&moveValue);		// (위 2.의 경우) 함수가 연속 실행되게
}

void TestQuad::Fire(void* value)
{
	if (curAmmo == 0) return;

	BulletManager::Get()->Fire(turret->GlobalPos(),
		turret->Right(), 0);

	//총알과 탄창이 적용된 경우라면
	--curAmmo;

	float amount = (float)curAmmo / (float)maxAmmo;
	mags->SetAmount(amount);
}

//---------------------------------------------------------------------------------

void TestQuad::SetController()
{
	controller = new ObjectController(this);

	/*
	controller->AddInput(
		"MoveUp", VK_UP, InputAction::InputType::PRESS, (void*)&DefaultFloat);
	controller->BindAction(
		"MoveUp", bind(&TestQuad::MoveUp, this, placeholders::_1));

	controller->AddInput(
		"MoveDown", VK_DOWN, InputAction::InputType::PRESS, (void*)&DefaultFloat);
	controller->BindAction(
		"MoveDown", bind(&TestQuad::MoveDown, this, placeholders::_1));

	controller->AddInput(
		"MoveLeft", VK_LEFT, InputAction::InputType::PRESS, (void*)&DefaultFloat);
	controller->BindAction(
		"MoveLeft", bind(&TestQuad::MoveLeft, this, placeholders::_1));

	controller->AddInput(
		"MoveRight", VK_RIGHT, InputAction::InputType::PRESS, (void*)&DefaultFloat);
	controller->BindAction(
		"MoveRight", bind(&TestQuad::MoveRight, this, placeholders::_1));
	*/

	// 벡터 입력의 경우

	controller->AddInput(
		"MoveUp", VK_UP, InputAction::InputType::PRESS, nullptr); // 스스로 이동값 주기
	controller->BindAction(
		"MoveUp",
		[this](void*) { MoveVector2(0, +1); }); // 람다식 적용
	// "MoveVector2를 쓰는 것이 값이다"

// 아래쪽
	controller->AddInput(
		"MoveDown", VK_DOWN, InputAction::InputType::PRESS, nullptr);
	controller->BindAction("MoveDown", [this](void*) { MoveVector2(0, -1); });

	// 왼쪽
	controller->AddInput(
		"MoveLeft", VK_LEFT, InputAction::InputType::PRESS, nullptr);
	controller->BindAction("MoveLeft", [this](void*) { MoveVector2(-1, 0); });

	// 오른쪽
	controller->AddInput(
		"MoveRight", VK_RIGHT, InputAction::InputType::PRESS, nullptr);
	controller->BindAction("MoveRight", [this](void*) { MoveVector2(+1, 0); });

	controller->AddInput(
		"RotateCW", 'E', InputAction::InputType::PRESS, (void*)&DefaultFloat);
	controller->BindAction(
		"RotateCW", bind(&TestQuad::RotateCW, this, placeholders::_1));

	controller->AddInput(
		"RotateCCW", 'Q', InputAction::InputType::PRESS, (void*)&DefaultFloat);
	controller->BindAction(
		"RotateCCW", bind(&TestQuad::RotateCCW, this, placeholders::_1));

	controller->AddInput(
		"Fire", VK_LBUTTON, InputAction::InputType::DOWN, (void*)&DefaultInt);
	controller->BindAction(
		"Fire", bind(&TestQuad::Fire, this, placeholders::_1));

	controller->AddInput(
		"MouseMoveOn", VK_RBUTTON, InputAction::InputType::DOWN, (void*)&DefaultInt
	);
	controller->BindAction(
		"MouseMoveOn", bind(&TestQuad::MouseMove, this, (void*)&bTRUE));

	controller->AddInput(
		"MouseMoveOff", VK_RBUTTON, InputAction::InputType::UP, (void*)&DefaultInt
	);
	controller->BindAction(
		"MouseMoveOff", bind(&TestQuad::MouseMove, this, (void*)&bFALSE));
}