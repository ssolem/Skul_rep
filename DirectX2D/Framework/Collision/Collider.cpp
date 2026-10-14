#include "Framework.h"

Collider::Collider()
{
    vertexShader = Shader::AddVS(L"VertexPos.hlsl");
    pixelShader = Shader::AddPS(L"PixelPos.hlsl");

    worldBuffer = new MatrixBuffer();
    colorBuffer = new ColorBuffer();
    colorBuffer->Get() = { 0, 1, 0, 1 };
}

Collider::~Collider()
{
    delete vertexBuffer;

    delete colorBuffer;
    delete worldBuffer;
}

void Collider::Render()
{
    if (!isActive) return;

    worldBuffer->Set(world);
    worldBuffer->SetVS(0);
    colorBuffer->SetPS(0);

    vertexBuffer->Set(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

    vertexShader->Set();
    pixelShader->Set();

    DC->Draw(vertices.size(), 0);
}

bool Collider::IsCollision(Collider* collider, Vector2* overlap)
{
    if (!Active()) return false;
    if (!collider->Active()) return false;

    switch (collider->type)
    {
    case Collider::Type::RECT:
        return IsRectCollision((RectCollider*)collider, overlap);        
    case Collider::Type::CIRCLE:
        return IsCircleCollision((CircleCollider*)collider);
    case Collider::Type::LINE:
        //선은 "선 vs 다른 도형"만 구현되어 있으므로 선 쪽에서 검사 (선 vs 선은 미지원)
        if (type == Type::LINE) return false;
        return collider->IsCollision(this, overlap);
    }

    return false;
}
