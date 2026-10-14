#pragma once
class LineCollider : public Collider
{
public:
	LineCollider(Vector2 startPos, Vector2 endPos, float hitRange = 10);
	~LineCollider();

    virtual bool IsPointCollision(Vector2 point) override;
    virtual bool PushCollider(Collider* collider) override;
    virtual bool IsRectCollision(class RectCollider* rect, Vector2* overlap) override;
    virtual bool IsCircleCollision(class CircleCollider* circle) override;

    void SetHitRange(float value) { hitRange = value; }

    //부모 없이 쓰면 월드 좌표 그대로 - 시야 검사처럼 매번 두 점을 옮겨가며 재사용할 때
    void SetPoints(Vector2 startPos, Vector2 endPos);
    Vector2 GetStartPos() { return startPos; }
    Vector2 GetEndPos() { return endPos; }

private:

    Vector2 startPos;
    Vector2 endPos;

    float hitRange;
};

