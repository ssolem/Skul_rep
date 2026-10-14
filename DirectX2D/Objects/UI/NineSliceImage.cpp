#include "Framework.h"
#include "NineSliceImage.h"

NineSliceImage::NineSliceImage(wstring file, Vector2 edgeUV) : edgeUV(edgeUV)
{
	// 알맹이 계산
	Vector2 startUV = edgeUV;
	Vector2 endUV = Vector2(1, 1) - edgeUV;

	// 알맹이 생성
	mainFrame = new Quad(file, startUV, endUV);

	// 분절 이미지 생성
	corners[0] = new Quad(file, Vector2(0, 0), startUV);
	corners[1] = new Quad(file, Vector2(endUV.x, 0), Vector2(1, startUV.y));
	corners[2] = new Quad(file, Vector2(0, endUV.y), Vector2(startUV.x, 1));
	corners[3] = new Quad(file, endUV, Vector2(1, 1));

	verEdges[0] = new Quad(file, Vector2(startUV.x, 0), Vector2(endUV.x, startUV.y));
	verEdges[1] = new Quad(file, Vector2(startUV.x, endUV.y), Vector2(endUV.x, 1));

	horEdges[0] = new Quad(file, Vector2(0, startUV.y), Vector2(startUV.x, endUV.y));
	horEdges[1] = new Quad(file, Vector2(endUV.x, startUV.y), Vector2(1, endUV.y));;

	mainFrame->SetParent(this);
	for (Quad* corner : corners) corner->SetParent(this);
	for (Quad* edge : verEdges) edge->SetParent(this);
	for (Quad* edge : horEdges) edge->SetParent(this);

	// 분절 이미지 위치 조정
	Vector2 textureSize = mainFrame->GetTexture()->GetSize();
	cornerSize = textureSize * edgeUV;
	srcCenterSize = textureSize - cornerSize * 2.0f;

	SetCenterSize(srcCenterSize);
}

NineSliceImage::~NineSliceImage()
{
	for (Quad* corner : corners)
		delete corner;

	for (Quad* edge : verEdges)
		delete edge;

	for (Quad* edge : horEdges)
		delete edge;

	delete mainFrame;
}

void NineSliceImage::Update()
{
	UpdateWorld();

	for (Quad* corner : corners)
		corner->UpdateWorld();

	for (Quad* edge : verEdges)
		edge->UpdateWorld();

	for (Quad* edge : horEdges)
		edge->UpdateWorld();

	mainFrame->UpdateWorld();
}

void NineSliceImage::Render()
{
	for (Quad* corner : corners)
		corner->Render();

	for (Quad* edge : verEdges)
		edge->Render();

	for (Quad* edge : horEdges)
		edge->Render();

	mainFrame->Render();
}

void NineSliceImage::SetCenterSize(Vector2 size)
{
	centerSize = size;

	Vector2 stretchSize = centerSize / srcCenterSize;
	Vector2 offset = (centerSize + cornerSize) * 0.5f;
	
	mainFrame->Scale() = stretchSize;
	mainFrame->Pos() = Vector2(0, 0);

	for (Quad* q : corners) q->Scale() = Vector2::One;
	corners[0]->Pos() = Vector2(offset.x * -1, offset.y * +1);
	corners[1]->Pos() = Vector2(offset.x * +1, offset.y * +1);
	corners[2]->Pos() = Vector2(offset.x * -1, offset.y * -1);
	corners[3]->Pos() = Vector2(offset.x * +1, offset.y * -1);

	//상하 테두리: 가로만 늘어남
	for (Quad* q : verEdges) q->Scale() = Vector2(stretchSize.x, 1.0f);
	verEdges[0]->Pos() = Vector2(0, offset.y * +1);
	verEdges[1]->Pos() = Vector2(0, offset.y * -1);

	//좌우 테두리: 세로만 늘어남
	for (Quad* q : horEdges) q->Scale() = Vector2(1.0f, stretchSize.y);
	horEdges[0]->Pos() = Vector2(offset.x * -1, 0);
	horEdges[1]->Pos() = Vector2(offset.x * +1, 0);
}
