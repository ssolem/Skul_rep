#pragma once

class Camera : public Transform
{
public:
    Camera();
    ~Camera();

    void Update();
    void SetView();

    void SetTarget(Transform* target) { this->target = target; }
    void SetOffset(float x, float y) { targetOffset = { x, y }; }
    void SetLeftBottom(float x, float y) { leftBottom = { x, y }; }
    void SetRightTop(float x, float y) { rightTop = { x, y }; }
    void SetDeviceSize(float x, float y) { deviceSize = { x, y }; }

    Vector2 ScreenToWorld(Vector2 pos);
    Vector2 WorldToScreen(Vector2 pos);

private:
    void FreeMode();
    void FollowMode();

    void FixPosition(Vector2& position);

private:
    MatrixBuffer* viewBuffer;
    Matrix view;

    float speed = 800.0f;

    Transform* target = nullptr;

public:
    Vector2 targetOffset = { CENTER_X, CENTER_Y };
    Vector2 leftBottom = { -WIN_WIDTH, -WIN_HEIGHT };
    Vector2 rightTop = { WIN_WIDTH * 2, WIN_HEIGHT * 2};
    Vector2 deviceSize = { WIN_WIDTH, WIN_HEIGHT };
};