#include "Framework.h"

RectCollider::RectCollider(Vector2 size)
    : size(size)
{
    type = Type::RECT;

    Vector2 halfSize = size * 0.5f;

    vertices.emplace_back(-halfSize.x, +halfSize.y);
    vertices.emplace_back(+halfSize.x, +halfSize.y);
    vertices.emplace_back(+halfSize.x, -halfSize.y);
    vertices.emplace_back(-halfSize.x, -halfSize.y);
    vertices.emplace_back(-halfSize.x, +halfSize.y);

    vertexBuffer = new VertexBuffer(vertices.data(),
        sizeof(VertexPos), vertices.size());
}

bool RectCollider::IsPointCollision(Vector2 point)
{
	// 받은 벡터 정보를 공간에 주입한다 -> 벡터와 공간을 '곱하면' 되는데...
	// 행렬곱 순서의 문제에 대응하기 위해, 먼저 원래의 공간을 역행렬로 한 번 뽑아낸다

	Matrix invWorld = XMMatrixInverse(nullptr, world); // <-공간은 자기 자신이 기준인 상태

	point *= invWorld; // <- 절대공간 업데이트 중에 지정된 곳이 어디인지 볼 수 있다

	// -> 이제 이 곳이 사각형 영역 안에 있는지만 보면 된다
	//    : "원점"에서 얼마나 떨어졌는지만 본다

	Vector2 half = size * 0.5f; // 원점 기준 경계 영역 찾고

	return abs(point.x) <= half.x && abs(point.y) <= half.y;
}

bool RectCollider::PushCollider(Collider* collider)
{
	if (!IsCollision(collider)) return false;

	Direction dir = GetDirection(collider); // 상대방과의 방향 관계

	Transform* object = collider->GetParent(); //충돌체의 주인

	switch (dir)
	{
	case GameMath::Direction::UP:
		object->Pos() += Up() * PUSH_SPEED * DELTA;
		break;
	case GameMath::Direction::DOWN:
		object->Pos() += Down() * PUSH_SPEED * DELTA;
		break;
	case GameMath::Direction::LEFT:
		object->Pos() += Left() * PUSH_SPEED * DELTA;
		break;
	case GameMath::Direction::RIGHT:
		object->Pos() += Right() * PUSH_SPEED * DELTA;
		break;
	}

	return true;
}

Direction RectCollider::GetDirection(Collider* collider)
{
    // (격자 기반이면 이런 경우도 생각할 수 있다)
	if (collider->GlobalPos() == GlobalPos())
		return Direction::NONE;

	// 기준점 좌표 찾기
	Vector2 leftTop = LeftTop() - GlobalPos();
	Vector2 rightTop = RightTop() - GlobalPos();

	// 위치관계와 기준 방향 찾기 (* A - B : A가 목적지, B가 연산항)
	Vector2 direction = collider->GlobalPos() - GlobalPos();

	// 기준점과 방향이 나온 상태에서,
	// '어느 쪽이 어느 쪽보다 왼쪽인가?'를 보려는데

	// X를 비교하면 : 보는 입장에서 직관적, 본체 입장에서 의미가 다를 수 있다
	// -> 본체 쪽에서 보면 : 방향과 각자의 기준점 관계가 더 중요할 수 있다

	// -> 따라서, 벡터 두 개로부터 방향의 관계를 새로 봐야 할 수 있다
	//    두 벡터와 '방향' -> 외적

	// 여기서 위치 벡터가 기준 방향 벡터 사이에 있으면
	// 한쪽 면이 외적 축 순서를 A-B로 본다면 다른 쪽은 똑같은 A-B로 봤을 때
	// 방향이 반대로 틀어지게 된다. (축의 배치 문제)

	float crossLeft = Cross(leftTop, direction); // 왼쪽 기준과 위치를 외적하면 예상되는 수직선의 길이 (즉 면의 방향)
	float crossRight = Cross(rightTop, direction); // 오른쪽 기준과 위치를 외적하면 예상되는 수직선의 길이 (즉 면의 방향)

	if (crossLeft * crossRight < 0) //외적면 방향이 반대인 경우, 위치 벡터가 왼쪽 오른쪽 사이를 파고든 경우 -> 위거나 아래
	{
		float dot = Dot(Up(), direction.GetNormalized()); //위치벡터 내 '위'벡터 사이의 길이 비율(점곱) 내적
		float angle = acos(dot);	//dot가 길이라면 예상 가능한 cos각도

		if (abs(angle) < XM_PIDIV2) // 그 각도가 내 입장에서 왼쪽 오른쪽 90도 이내면
			return Direction::UP;
		else
			return Direction::DOWN;
	}
	else //면 방향 끼리의 곱이기 때문에 0은 없다.
	{
		float dot = Dot(Right(), direction.GetNormalized()); 
		float angle = acos(dot);

		if (abs(angle) < XM_PIDIV2)
			return Direction::RIGHT;
		else
			return Direction::LEFT;
	}
}

void RectCollider::SetSize(Vector2 newSize)
{
	if (size == newSize) return;

	size = newSize;

	Vector2 halfSize = size * 0.5f;

	vertices[0] = { -halfSize.x, +halfSize.y };
	vertices[1] = { +halfSize.x, +halfSize.y };
	vertices[2] = { +halfSize.x, -halfSize.y };
	vertices[3] = { -halfSize.x, -halfSize.y };
	vertices[4] = { -halfSize.x, +halfSize.y };

	vertexBuffer->Update(vertices.data(), vertices.size());
}

bool RectCollider::IsRectCollision(RectCollider* rect, Vector2* overlap)
{
	// 교집합 데이터를 요구받았으면
	if (overlap) return IsAABB(rect, overlap);

	// 아니면 정렬을 했든 안 했든간에 그냥 축검사하면 그만
	return IsOBB(rect);
}

bool RectCollider::IsCircleCollision(CircleCollider* circle)
{
	// 십자가 안이면 서로 수직 길이 맞추기 (최대 길이 비교)
	// 십자가 밖이면 꼭짓점과 원 반지름 비교하기

	// -> 사각형이라는 입장에서 보면 : 관측축 입장에서 단순비교 가능하냐, 점곱 비교가 필요하냐?

	// 그러므로 일단 위치관계 (점곱을 할 관계 벡터)

	ObbDesc obb = GetObb(); // 사각형의 현재 기하정보 추출
	Vector2 direction = obb.position - circle->GlobalPos();

	// 이 관계가 관측축 기준에서 어떤 선이(혹은 어떤 길이가) 되는지
	float x = abs(Dot(direction, obb.axis[0]));
	float y = abs(Dot(direction, obb.axis[1]));

	// 일단 이 길이가 '반지름'끼리 비교해도 벗어날 정도인가? -> 완전 떨어짐
	if (obb.halfSize.x + circle->Radius() < x) return false;
	if (obb.halfSize.y + circle->Radius() < y) return false;

	// 두 중심 거리가 아예 사각형 가로나 세로보다도 짧다 -> 아주 겹쳐짐
	if (x < obb.halfSize.x) return true;
	if (y < obb.halfSize.y) return true;

	// -> 가장 가까운 '후보 접점'과 그 거리를 찾는다
	Vector2 tempVec = Vector2(x, y) - obb.halfSize;

	// 그 '후보 접점'이 반지름보다는 안에 있으면 된다 (그러면 사각형 꼭짓점 중 하나가 원 안에 들어감)
	return tempVec.Length() < circle->Radius();
}

Vector2 RectCollider::LeftTop()
{
	Vector2 edge = Vector2(-size.x, +size.y) * 0.5f;
	return edge * world; // '* world' : 절대 공간 행렬 -> 사물의 크기 반영
}

Vector2 RectCollider::LeftBottom()
{
	Vector2 edge = Vector2(-size.x, -size.y) * 0.5f;
	return edge * world;
}

Vector2 RectCollider::RightTop()
{
	Vector2 edge = Vector2(+size.x, +size.y) * 0.5f;
	return edge * world;
}

Vector2 RectCollider::RightBottom()
{
	Vector2 edge = Vector2(+size.x, -size.y) * 0.5f;
	return edge * world;
}

float RectCollider::L()
{
	float minLeft = min(LeftTop().x, LeftBottom().x); // 왼쪽이던 것 중에 더 왼쪽
	float minRight = min(RightTop().x, RightBottom().x); // 오른쪽 중에 더 왼쪽

	//둘 중에서 더 왼쪽인 게 지금의 왼쪽
	return min(minLeft, minRight);
}

float RectCollider::R()
{
	float maxLeft = max(LeftTop().x, LeftBottom().x); // 더 오른쪽
	float maxRight = max(RightTop().x, RightBottom().x); // 더 오른쪽

	// 둘 중 오른쪽이 오른쪽
	return max(maxLeft, maxRight);
}

float RectCollider::T()
{
	float maxTop = max(LeftTop().y, RightTop().y);
	float maxBottom = max(LeftBottom().y, RightBottom().y);

	return max(maxTop, maxBottom);
}

float RectCollider::B()
{
	float minTop = max(LeftTop().y, RightTop().y);
	float minBottom = max(LeftBottom().y, RightBottom().y);

	return min(minTop, minBottom);
}

RectCollider::ObbDesc RectCollider::GetObb()
{
	// 위치와 축, 크기 정보 기입
	obbDesc.position = GlobalPos();
	obbDesc.halfSize = Half();

	obbDesc.axis[0] = Right(); // x축
	obbDesc.axis[1] = Up();    // y축

    return obbDesc;
}

bool RectCollider::IsAABB(RectCollider* rect, Vector2* overlap)
{
	// 두 상자의 '확장된 변'끼리 위치비교

	// 목표 -> 교집합이 존재하는지 따져보기 (사이에 낀 정보로 도형이 나오는지)
	float minRight = min(R(), rect->R()); // 오른쪽 중에서 더 왼쪽
	float maxLeft = max(L(), rect->L());
	float minTop = min(T(), rect->T());
	float maxBottom = max(B(), rect->B());

	// 위의 '변' 정보로 직사각형을 구성하는 가로, 세로 정보를 수집한다

	overlap->x = minRight - maxLeft;
	overlap->y = minTop - maxBottom;

	// 여기서 '가로'와 '세로'가 성립한다면

	if (overlap->x > 0 && overlap->y > 0) return true; // 충돌

    return false; // 아니면, 충돌 아님.
}

bool RectCollider::IsOBB(RectCollider* rect)
{   
    // 두 상자의 OBB 충돌 계산

	// 먼저 두 상자의 공간 정보를 따로 기입하고
	ObbDesc obbA = GetObb();
	ObbDesc obbB = rect->GetObb();

	// 각 상자의 x, y 축마다 (4번) 두 상자의 정보를 서로 비교해서
	if (IsSeperate(obbA.axis[0], obbA, obbB)) return false;
	if (IsSeperate(obbA.axis[1], obbA, obbB)) return false;
	if (IsSeperate(obbB.axis[0], obbA, obbB)) return false;
	if (IsSeperate(obbB.axis[1], obbA, obbB)) return false;

	// -> 네 번을 검사했는데, 축에 걸쳐지는 꼭지점이 계속 1개는 나온다고 판별되면?
	//    진짜로 어디 한 군데는 걸쳐진 거라고 봐도 무방하다

	return true; // 죽, 충돌 상황
}

bool RectCollider::IsSeperate(Vector2 seperateAxis, ObbDesc box1, ObbDesc box2)
{
	// 축분리에 의한 접점이 있는지 확인하는 과정

	// 함수 안에서는 축 1개를 이미 기준이 됐다고 치고
	// 다른 쪽 상자가 해당 기준축에 닿을 수 있는지만 본다

	// -> 기준축과, 상대 상자의 꼭지점을 보겠다는 의미

	// -------------------------------------------------------------

	// 1. 축 기준에서 두 상자의 거리
	
	float distance = abs(Dot(seperateAxis, box1.position - box2.position));

	// 2. 축 안에서 기준이 된 상자의 경계 (기준자료)

	Vector2 right = box1.axis[0] * box1.halfSize.x; // x쪽
	Vector2 up = box1.axis[1] * box1.halfSize.y;    // y쪽

	float a = abs(Dot(seperateAxis, right)) + abs(Dot(seperateAxis, up));

	// 3. 상대 상자에서 해당 축에 제일 가까이 갈 것 같은 그 꼭지점, 의 점곱 자료

	right = box2.axis[0] * box2.halfSize.x;
	up = box2.axis[1] * box2.halfSize.y;

	float b = abs(Dot(seperateAxis, right)) + abs(Dot(seperateAxis, up));

	// 1 + 2 + 3. a 값과 b값을 더한 값이 거리를 못 넘으면,
	//            이 축에 대한 두 상자 관계는 '분리'

	return distance > a + b;

	// -> 다른 축에서는 접점이 생겼을 수도 있으니 축을 바꿔 가면서 계속 보면 된다
}
