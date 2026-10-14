#include "Framework.h"
#include "LineCollider.h"

LineCollider::LineCollider(Vector2 startPos, Vector2 endPos, float hitRange)
	:startPos(startPos), endPos(endPos), hitRange(hitRange)
{
	type = Type::LINE;

	//선이 줄 만한 모양? winAPI나 , DX나 크게 다르지도 않다.
	// - 시작 점 , 끝 점 , 선분으로 긋기

	vertices.emplace_back(startPos.x, startPos.y);
	vertices.emplace_back(endPos.x, endPos.y);

	vertexBuffer = new VertexBuffer(vertices.data(), sizeof(VertexPos), vertices.size());
}

LineCollider::~LineCollider()
{
}

void LineCollider::SetPoints(Vector2 startPos, Vector2 endPos)
{
	this->startPos = startPos;
	this->endPos = endPos;

	vertices[0] = VertexPos(startPos.x, startPos.y);
	vertices[1] = VertexPos(endPos.x, endPos.y);
	vertexBuffer->Update(vertices.data(), vertices.size());
}

bool LineCollider::IsPointCollision(Vector2 point)
{
	//두 벡터 : 선의 시작 입장에서 선 자체(관측을 위한 벡터)와
	//			점으로 가는 방향(빗변이 될, 관측 대상 벡터)를 모두 구한다.

	Vector2 line = endPos - startPos;
	Vector2 toPoint = point - startPos;

	//선 자체의 도형 확인

	float lineSquare = line.x * line.x + line.y * line.y;
	if (lineSquare <= FLT_EPSILON)
	{
		//사실상 점과 점의 비교

		Vector2 distPoint = point - startPos;
		float distPointSquare = distPoint.x * distPoint.x + distPoint.y * distPoint.y;

		return distPointSquare <= hitRange * hitRange;
	}

	//선 입장에서 점으로 가는 벡터를 보면 나오는 길이 관계 -> 점곱과 다시 그 비율

	float t = Dot(toPoint, line) / lineSquare;
	
	//혹시라도 이 '비율'이 선분의 벡터 비율을 넘어간다면 강제로 최대, 최소 안으로 가져온다.

	t = Clamp(0, 1, t); //클램핑 범위 제한

	// -> 이 비율을 시작지점에서 적용하면 접점의 후보가 나올 것 = 직교점

	Vector2 closePoint = startPos + line * t;

	Vector2 distLine = closePoint - point; //지정된 점에서 선으로 가는 최단거리 벡터

	float distLineSquare = distLine.x * distLine.x + distLine.y * distLine.y;

	return distLineSquare <= hitRange * hitRange;
}

bool LineCollider::PushCollider(Collider* collider)
{
	return false;
}

//선분 vs 사각형(OBB) - 선분을 사각형의 로컬 축으로 옮긴 뒤 축별 구간(slab)이 겹치는지 검사
//startPos/endPos는 이 콜라이더의 로컬 좌표 = 부모 없이 쓰면 월드 좌표
bool LineCollider::IsRectCollision(RectCollider* rect, Vector2* overlap)
{
	RectCollider::ObbDesc obb = rect->GetObb();

	Vector2 start = startPos - obb.position;
	Vector2 end = endPos - obb.position;

	float origin[2] = { Dot(start, obb.axis[0]), Dot(start, obb.axis[1]) };
	float delta[2] = { Dot(end, obb.axis[0]) - origin[0], Dot(end, obb.axis[1]) - origin[1] };
	float half[2] = { obb.halfSize.x, obb.halfSize.y };

	float tMin = 0.0f;
	float tMax = 1.0f;

	for (int i = 0; i < 2; ++i)
	{
		//이 축과 평행한 선분 - 시작점이 구간 밖이면 절대 안 닿음
		if (abs(delta[i]) < FLT_EPSILON)
		{
			if (abs(origin[i]) > half[i]) return false;
			continue;
		}

		float t1 = (-half[i] - origin[i]) / delta[i];
		float t2 = (half[i] - origin[i]) / delta[i];
		if (t1 > t2) swap(t1, t2);

		tMin = max(tMin, t1);
		tMax = min(tMax, t2);
		if (tMin > tMax) return false;
	}

	return true;
}

bool LineCollider::IsCircleCollision(CircleCollider* circle)
{
	return false;
}
