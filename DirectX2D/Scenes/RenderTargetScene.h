#pragma once

class TestQuad;
class TestBullet;

class RenderTargetScene : public Scene
{
public:
    RenderTargetScene();
    ~RenderTargetScene();

    virtual void Update() override;
    virtual void PreRender() override;
    virtual void Render() override;
    virtual void PostRender() override;

private:

    //테스트용 그림들
    Quad* background;
    TestQuad* airplane;

    //렌더 타겟 담당자
    RenderTarget* renderTarget;

    //만들어진, 만드는 RT 텍스쳐 기록할 곳
    vector<Quad*> renderTextures;

    //렌더타겟 중의 옵션 변경용
    IntValueBuffer* intValueBuffer;
    FloatValueBuffer* floatValueBuffer;

};

