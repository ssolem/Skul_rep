#pragma once

class RectCollider : public Collider
{
public:
    friend class CircleCollider; // 클래스 간 결연 (충돌체 사이에서는 private 접근권 확보)

    struct ObbDesc
    {
        Vector2 position;
        Vector2 axis[2];
        Vector2 halfSize;
    };

    //-> 방향은 본 프로젝트에서는 별도 헤더에 미리 작성된 상태

public:
    RectCollider(Vector2 size);
    ~RectCollider() = default;

    virtual bool IsPointCollision(Vector2 point) override;
	virtual bool PushCollider(Collider* collider) override;

    Direction GetDirection(Collider* collider);

    void SetSize(Vector2 newSize);

    Vector2 Size() { return size * GlobalScale(); }
    Vector2 Half() { return Size() * 0.5f; }

    Vector2 LeftTop();
    Vector2 LeftBottom();
    Vector2 RightTop();
    Vector2 RightBottom();

    float L();
    float R();
    float T();
    float B();

    ObbDesc GetObb();

protected:

    virtual bool IsRectCollision(RectCollider* rect, Vector2* overlap) override;
    virtual bool IsCircleCollision(CircleCollider* circle) override;

    bool IsAABB(RectCollider* rect, Vector2* overlap);
    bool IsOBB(RectCollider* rect);

    bool IsSeperate(Vector2 seperateAxis, ObbDesc box1, ObbDesc box2);

private:

    Vector2 size;

    ObbDesc obbDesc;       
    
};