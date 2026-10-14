#pragma once

class SkulMapGrid : public Transform
{
public:
	SkulMapGrid();
	~SkulMapGrid();

	void CreateGrid(Vector2 tileSize);
	void Update();
	void Render();

	void SetColor(Float4 color) { colorBuffer->Get() = color; }

private:
	Vector2 tileSize;

	VertexBuffer* vertexBuffer;
	vector<VertexPos> vertices;

	VertexShader* vertexShader;
	PixelShader* pixelShader;
	MatrixBuffer* worldBuffer;
	ColorBuffer* colorBuffer;
};

