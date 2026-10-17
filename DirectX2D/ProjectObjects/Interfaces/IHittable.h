#pragma once

class Collider;

class IHittable
{
public:
    virtual ~IHittable() = default;
    virtual void OnHit(const HitData& hit) = 0;
    virtual Vector2 GetHitPos() = 0;	//넉백 방향 판정용 - 공격 판정 위치와 비교
};