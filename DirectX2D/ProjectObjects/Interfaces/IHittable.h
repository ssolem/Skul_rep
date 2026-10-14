#pragma once

class Collider;

class IHittable
{
public:
    virtual ~IHittable() = default;
    virtual void OnHit(const HitData& hit) = 0;
};