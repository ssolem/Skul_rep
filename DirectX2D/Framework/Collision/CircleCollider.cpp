#include "Framework.h"
#include "CircleCollider.h"
#include "RectCollider.h"

CircleCollider::CircleCollider(float radius)
	: radius(radius)
{
	type = Type::CIRCLE;

	vertices.reserve(VERTEX_COUNT + 1);

	// 한 번 돌 때마다 돌아갈 각도
	float theta = XM_2PI / VERTEX_COUNT;
	for (UINT i = 0; i <= VERTEX_COUNT; ++i)
	{
		float x = cos(theta * i) * radius;
		float y = sin(theta * i) * radius;

		vertices.emplace_back(x, y); // 각도마다 좌표를 구해서 정점 벡터에 심는다
	}

	vertexBuffer = new VertexBuffer(vertices.data(), sizeof(VertexPos), vertices.size());
}

CircleCollider::~CircleCollider()
{
}

bool CircleCollider::IsPointCollision(Vector2 point)
{
	float distance = Distance(point, GlobalPos()); // 점과 중심 사이의 거리
    return distance < Radius(); // 거리가 반지름 안이면 다 충돌
}

bool CircleCollider::PushCollider(Collider* collider)
{
    return false;
}

bool CircleCollider::IsRectCollision(RectCollider* rect, Vector2* overlap)
{
    //return rect->IsCollision(this); 
	
	return rect->IsCircleCollision(this); // 사각형한테 충돌 체크 떠넘기기
}

bool CircleCollider::IsCircleCollision(CircleCollider* circle)
{
	float distance = Distance(circle->GlobalPos(), GlobalPos()); // 중심 사이의 거리
	return distance < Radius(); // 거리가 반지름 안이면 다 충돌
}
