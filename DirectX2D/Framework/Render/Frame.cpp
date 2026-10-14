#include "Framework.h"
#include "Frame.h"

Frame::Frame(wstring textureFile, Vector2 startUV, Vector2 endUV, Vector2 pos)
{
	// 일단 그림을 앱에 등록하고 나도 가져온다
	texture = Texture::Add(textureFile); // 파일에서 그림 데이터 뽑기

	//잘라낼 크기를 UV에서 추출한다
	//size = endUV - startUV;
	//size *= texture->GetSize();
	size = (endUV - startUV) * texture->GetSize();

	// 정점 추출
	float left = pos.x - size.x * 0.5f;
	float right = pos.x + size.x * 0.5f;
	float top = pos.y + size.y * 0.5f;
	float bottom = pos.y - size.y * 0.5f;

	vertices.emplace_back(left, top, startUV.x, startUV.y); // IL에 데이터 채우기를 직접
	vertices.emplace_back(right, top, endUV.x, startUV.y);
	vertices.emplace_back(left, bottom, startUV.x, endUV.y);
	vertices.emplace_back(right, bottom, endUV.x, endUV.y);

	indices = { 0, 1, 2, 2, 1, 3 };
	vertexBuffer = new VertexBuffer(vertices.data(), sizeof(Vertex), vertices.size());
	indexBuffer = new IndexBuffer(indices.data(), indices.size());
}

Frame::Frame(wstring textureFile, float x, float y, float width, float height)
{
	texture = Texture::Add(textureFile);

	// 직접적인 데이터 대신 직관적인 좌표와 '크기 표현'을 받은 거니까...

	size = { width, height }; // 크기 표현에서 벡터 '크기'를 바로 만들자

	// 여기서 중심에서의 기준거리가 있으면 나중에 꼭짓점, UV 등에 쓸 수 있다
	// -> 나중에 그 중심의 좌표를 x, y로 정하면 된다

	Vector2 halfSize = size * 0.5f; // '중심'을 두기 위해 크기를 반토막 내고

	// 중심을 전제한 기준거리를 또 낸다
	float left =	 halfSize.x * -1;
	float right =	 halfSize.x * +1;
	float top =		 halfSize.y * +1;
	float bottom =	 halfSize.y * -1;

	// 위치와 크기로 프레임을 만들더라도 어차피 컴퓨터에게 정점을 줄 때는 벡터와...UV가 필요
	Vector2 startUV = Vector2(x, y) / texture->GetSize(); // 각 좌표를 크기별로 나눈다
	Vector2 frameSize = size / texture->GetSize();		  // 크기도 "정규화"
	Vector2 endUV = startUV + frameSize; // 시작 '좌표'에서 액자 크기만큼 가면 거기가 끝

	vertices.emplace_back(left, top, startUV.x, startUV.y); // IL에 데이터 채우기를 직접
	vertices.emplace_back(right, top, endUV.x, startUV.y);
	vertices.emplace_back(left, bottom, startUV.x, endUV.y);
	vertices.emplace_back(right, bottom, endUV.x, endUV.y);

	indices = { 0, 1, 2, 2, 1, 3 };
	vertexBuffer = new VertexBuffer(vertices.data(), sizeof(Vertex), vertices.size());
	indexBuffer = new IndexBuffer(indices.data(), indices.size());
}

Frame::~Frame()
{
	delete vertexBuffer;
	delete indexBuffer;

	// -> 텍스처는 정적 클래스 데이터로 받을 거라, 여기서 지울 대상 아님
}

void Frame::Render()
{
	vertexBuffer->Set();
	indexBuffer->Set();

	texture->PSSet(); // 픽셀 셰이더

	DC->DrawIndexed(indices.size(), 0, 0);
}