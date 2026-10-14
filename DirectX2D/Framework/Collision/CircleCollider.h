#pragma once

class CircleCollider : public Collider
{
private:
	friend class RectCollider; // 클래스 간 결연 (충돌체 사이에서는 private 접근권 확보)

	const UINT VERTEX_COUNT = 64;

public:
	CircleCollider(float radius = 1); 
	~CircleCollider();

	virtual bool IsPointCollision(Vector2 point) override;
	virtual bool PushCollider(Collider* collider) override;

	float Radius() { return radius * max(GlobalScale().x, GlobalScale().y); }

protected:

	virtual bool IsRectCollision(class RectCollider* rect, Vector2* overlap = nullptr) override;
	virtual bool IsCircleCollision(class CircleCollider* circle) override;

private:
	float radius;
};

