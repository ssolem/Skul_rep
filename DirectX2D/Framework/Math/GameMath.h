#pragma once

namespace GameMath
{
    const float DefaultFloat = 1;
    const float DefaultInt = 1;

	const float GRAVITY = 980.0f;

    enum class Direction
    {
        UP, DOWN, LEFT, RIGHT, NONE
    };

    struct Vector2IntLess
    {
        bool operator()(const Vector2& a, const Vector2& b) const
        {
            if ((int)a.x != (int)b.x) return (int)a.x < (int)b.x;
            return (int)a.y < (int)b.y;
        }
    };

    int Random(const int& min, const int& max);
    float Random(const float& min, const float& max);

    float Clamp(const float& min, const float& max, float value);

    float Lerp(const float& start, const float& end, float t);
    Vector2 Lerp(const Vector2& start, const Vector2& end, float t);    
    Vector2 SLerp(const Vector2& start, const Vector2& end, float t);    
    
    float Cross(const Vector2& vec1, const Vector2& vec2);
    float Dot(const Vector2& vec1, const Vector2& vec2);

    float Distance(const Vector2& vec1, const Vector2& vec2);
}