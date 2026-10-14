#pragma once

class ImageFont : public Transform //폰트는 이미지이기 떄문에 위치가 필요
{
	//왜 통그림이 아니냐면 - 각각의 글자를 하나의 공통 이미지에서 잘라서 사용

public:
	enum class AlignedType
	{
		LEFT, CENTER, RIGHT
	};

private:
	const UINT MAX_LENGTH = 8; //폰트 리소스마다 차이는 있지만
								// 길이 제한 자체를 필수

public:
	ImageFont(wstring textureFile, float interval = 1.0f);
	~ImageFont();

	void Update();
	void Render();

	void SetValue(UINT value) { this->value = value; }
	void SetAlignedType(AlignedType type) { alignedType = type; }

private:
	UINT value = 0;		//실질 텍스트...를 위한 기본 값
	string strValue;	//실질적인 텍스트 내용
	
	//텍스트 이미지를 대응하기 위한 데이터
	vector<Quad*> quads;
	vector<Texture*> textures;
	IntValueBuffer* valueBuffer;

	//글꼴 데이터
	AlignedType alignedType;
	Vector2 fontSize;

	//간격 변수
	float interval;
};