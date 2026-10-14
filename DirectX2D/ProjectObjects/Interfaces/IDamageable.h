#pragma once

class IDamageable
{
public:
    virtual ~IDamageable() = default;
    virtual void TakeDamage(int amount) = 0;
};