#pragma once

class Frame
{
public:

	Frame(						// 버전 1 생성자 : 그림과 UV로 프레임 생성
		wstring textureFile,
		Vector2 startUV = Vector2(0, 0), Vector2 endUV = Vector2(1, 1),
		Vector2 pos = Vector2()
	);

	Frame(						// 버전 2 생성자 : 프레임 크기로 생성
		wstring textureFile,
		float x, float y, float width, float height
	);

	~Frame();

	void Render();

	Vector2 GetSize() { return size; }

private:

	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;

	vector<Vertex> vertices;
	vector<UINT> indices;

	Texture* texture;
	Vector2 size;

};

