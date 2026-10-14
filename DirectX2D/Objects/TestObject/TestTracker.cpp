#include "Framework.h"
#include "TestTracker.h"

TestTracker::TestTracker()
	: Quad(L"Textures/Shooting/airplane.png")
{
	Pos() = { 50, 50 };
	Scale() *= 0.5f;

	collider = new RectCollider(texture->GetSize());
	collider->Scale() *= 0.5f;
	collider->SetParent(this);
}

TestTracker::TestTracker(bool useNodes)
	: Quad(L"Textures/Shooting/airplane.png"), useNodes(useNodes)
{
	Pos() = { 50, 50 };
	Scale() *= 0.5f;

	collider = new RectCollider(texture->GetSize());
	collider->Scale() *= 0.5f;
	collider->SetParent(this);
}

TestTracker::~TestTracker()
{
	delete collider;
}

void TestTracker::Update()
{
	switch (useNodes)
	{
	case true: TrackNodes();
		break;
	case false: TrackPath();
		break;
	}

	Quad::UpdateWorld();
	collider->UpdateWorld();
}

void TestTracker::Render()
{
	Quad::Render();
	collider->Render();
}

void TestTracker::PostRender()
{
	Quad::RenderUI();
}

void TestTracker::TrackPath()
{
	// 경로가 있다고 치면 그 경로를 쫓아가는 기능

	if (path.empty()) return;

	Vector2 direction = path.back() - Pos();
	Rot().z = direction.Angle();

	//데익스트로 클래스에서 경로를 역순으로 담았으므로
	//이 경로에서 맨 뒤라는 건 결국 시작지점부터 라는 표현

	Pos() += Right() * MOVE_SPEED * DELTA;

	if (direction.Length() < 0.1f) //목적지에 근접
		path.pop_back();
}

void TestTracker::TrackNodes()
{
	if (path.empty()) return;

	//노드 맵은 간격이 좁아서, 목표를 잘못 지나치기 쉬우므로
	// 목표 지점을 설정하는 대신 반대로 목표로 갈 거리를 먼저 설정하는것도 ok

	Vector2 target = path.back();
	float distance = Distance(Pos(), target); //목적지까지의 거리, 진행될 수록 줄어든다.

	float tickDistance = MOVE_SPEED * DELTA; //지금 비행기의 순간 속도, 실행 속도에 따라 달라짐

	if (distance <= tickDistance)
	{
		Pos() = target;
		path.pop_back();
		return;
	}

	//여기로 왔다면 남은 거리가 있는 상황
	Vector2 direction = (target - Pos()).GetNormalized();
	Rot().z = direction.Angle();

	//Pos() += direction * MOVE_SPEED * DELTA;
	Pos() += direction * tickDistance;
}
