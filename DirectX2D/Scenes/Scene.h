#pragma once

class Scene
{
public:
    virtual ~Scene() = default;

    virtual void Update() = 0;    

    virtual void PreRender() {}
    virtual void Render() = 0;
    virtual void PostRender() = 0;

    //매니저 대응용 시작, 종료 관리 함수
    // -> 올라가는 것과 시작되는 것이 별개일 수 있도록

    virtual void Start() {}
    virtual void End() {}
    virtual void Reset() {}

    //Start, End에 들어갈것들
    // 생성자에 들어갈 것들의 일부가 Start에 들어가면 된다.
};