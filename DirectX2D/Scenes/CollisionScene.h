#pragma once

class CollisionScene : public Scene
{
public:
    CollisionScene();
    ~CollisionScene();

    virtual void Update() override;
    virtual void Render() override;
    virtual void PostRender() override;

private:
    class TestQuad* testPlayer;
	class TestTarget* testTarget[2];
};