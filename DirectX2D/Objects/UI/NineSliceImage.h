#pragma once
class NineSliceImage : public GameObject
{
	// 9분절 이미지

public:
	NineSliceImage(wstring file, Vector2 edgeUV);
	~NineSliceImage();

	void Update();
	void Render();

	void SetCenterSize(Vector2 size);

	Vector2 GetCenterSize() { return centerSize; }
	Vector2 GetCornerSize() { return cornerSize; }
	Vector2 GetTotalSize() { return centerSize + cornerSize * 2.0f; }

	//private:
public:

	Vector2 edgeUV;	// 원본 그림에서 모퉁이 하나의 UV

	Vector2 cornerSize;     // 원본 그림에서 모퉁이 하나의 픽셀 크기
	Vector2 srcCenterSize;  // 원본 그림에서 가운데의 픽셀 크기
	Vector2 centerSize;     // 현재 늘어난 가운데 크기

	// 구성 이미지
	Quad* corners[4];	// 모퉁이
	Quad* verEdges[2];	// 테두리 - 상하
	Quad* horEdges[2];  // 테두리 - 좌우

	Quad* mainFrame;	// 중간 알맹이
};
