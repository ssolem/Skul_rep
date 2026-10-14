#pragma once

class Transform
{
public:
    Transform();
    ~Transform() = default;

    void UpdateWorld();
    void RenderUI();

    Vector2 Right() { return right.GetNormalized(); }
    Vector2 Left() { return right.GetNormalized() * -1.0f; }
    Vector2 Up() { return up.GetNormalized(); }
    Vector2 Down() { return up.GetNormalized() * -1.0f; }

    Vector2 GlobalPos() { return globalPosition; }
    Vector2 GlobalScale() { return globalScale; }

    Transform* GetParent();

    void SetParent(Transform* transform) { parent = transform; }
    void SetPivot(Vector2 pivot) { this->pivot = pivot; }

    bool Active();
    bool ActiveSelf() { return isActive; }

    void SetActive(bool isActive) { this->isActive = isActive; }

    void SetTag(string tag) { this->tag = tag; }

    Vector2& Pos() { return localPosition; }
    Float3& Rot() { return localRotation; }
    Vector2& Scale() { return localScale; }

    void Save();
    void Load();

    //공개용인 대신 접근이 까다로운 아토믹 변수
    atomic<int> stateCode;

    //아토믹 변수 : 정의로는 연산의 원자화, 실질적으로는 연산 단계의 통합에 가깝다.
    //              호출이 일어나면 해당 참조가 끝나는(다른 데이터에 대한 참조 요청이 오는) 
    //              그 순간까지 중간 연산이 모두 호출의 일부인 것으로 처리해
    //              연산 도중에 다른 데이터, 다른 객체, 다른 스레드가 접근하지 못하게

protected:
    string tag;

    Vector2 localPosition;
    Float3 localRotation = { 0, 0, 0 };
    Vector2 localScale = { 1, 1 };

    bool isActive = true;    

    Matrix world;

private:
    Transform* parent = nullptr;

    Matrix S, R, T;
    Matrix P, IP;
    Vector2 pivot;

    Vector2 right, up;
    Vector2 globalPosition, globalScale;

    XMFLOAT4X4 matWorld;
    
};