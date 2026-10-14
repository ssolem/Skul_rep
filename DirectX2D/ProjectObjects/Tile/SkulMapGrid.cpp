#include "Framework.h"
#include "SkulMapGrid.h"

SkulMapGrid::SkulMapGrid()
{
	vertexShader = Shader::AddVS(L"VertexPos.hlsl");
	pixelShader = Shader::AddPS(L"PixelPos.hlsl");

	worldBuffer = new MatrixBuffer();
	colorBuffer = new ColorBuffer();
	colorBuffer->Get() = { 0, 1, 0, 1 };
}

SkulMapGrid::~SkulMapGrid()
{
	delete vertexBuffer;
	delete colorBuffer;
	delete worldBuffer;
}

void SkulMapGrid::CreateGrid(Vector2 tileSize)
{
	this->tileSize = tileSize;

	vertices.clear();

	int halfX = (int)ceilf(CAM->deviceSize.x / tileSize.x / 2.0f) + 2;
	int halfY = (int)ceilf(CAM->deviceSize.y / tileSize.y / 2.0f) + 2;

	float left = -halfX * tileSize.x;
	float right = halfX * tileSize.x;
	float top = halfY * tileSize.y;
	float bottom = -halfY * tileSize.y;

	for(int x = -halfX; x <= halfX; ++x)
	{
		float posX = x * tileSize.x;
		vertices.push_back(VertexPos(posX, top));
		vertices.push_back(VertexPos(posX, bottom));
	}

	for(int y = -halfY; y <= halfY; ++y)
	{
		float posY = y * tileSize.y;
		vertices.push_back(VertexPos(left, posY));
		vertices.push_back(VertexPos(right, posY));
	}

	delete vertexBuffer;
	vertexBuffer = new VertexBuffer(vertices.data(), sizeof(VertexPos), vertices.size());
}

void SkulMapGrid::Update()
{
	if (vertexBuffer == nullptr)
		return;

	Vector2 center = CAM->ScreenToWorld(Vector2(WIN_WIDTH / 2.0f, WIN_HEIGHT / 2.0f));

	Pos().x = floorf(center.x / tileSize.x) * tileSize.x;
	Pos().y = floorf(center.y / tileSize.y) * tileSize.y;

	UpdateWorld();
}

void SkulMapGrid::Render()
{
	if (vertexBuffer == nullptr) return;

	worldBuffer->Set(world);
	worldBuffer->SetVS(0);
	colorBuffer->SetPS(0);

	vertexBuffer->Set(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	vertexShader->Set();
	pixelShader->Set();

	DC->Draw((UINT)vertices.size(), 0);
}