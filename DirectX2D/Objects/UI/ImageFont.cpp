#include "Framework.h"
#include "ImageFont.h"

ImageFont::ImageFont(wstring textureFile, float interval)
	: interval(interval)
{
	quads.resize(MAX_LENGTH);

	for (UINT i = 0; i < MAX_LENGTH; ++i)
	{
		//판 자체는 미리 만들어두고 다음에 그림 받기
		quads[i] = new Quad(textureFile, Vector2(), Vector2(0.1f, 1.0f));
		quads[i]->SetParent(this);
		quads[i]->SetPixelShader(L"ImageFont.hlsl"); //나중에 작성
	}
	fontSize = quads[0]->GetSize();
	valueBuffer = new IntValueBuffer();
}

ImageFont::~ImageFont()
{
	for (Quad* quad : quads) delete quad;
	delete valueBuffer;
}

void ImageFont::Update()
{
	//값이 뭐라고 바뀌든 거기서 실질 텍스트를 도출
	strValue = to_string(value);

	float start = 0.0f;
	float intervalSize = interval * (strValue.size() - 1);

	if (strValue.size() > 1) //두글자 이상
	{
		switch (alignedType) //정렬의 기준
		{
		case ImageFont::AlignedType::RIGHT:
			start = fontSize.x * strValue.size() + intervalSize;
			break;
		case ImageFont::AlignedType::CENTER:
			start = (fontSize.x * strValue.size() + intervalSize) * 0.5f;
			break;
			//왼쪽은 설정안해도 0.0f;
		}
	}

	for (UINT i = 0; i < strValue.size(); ++i)
	{
		quads[i]->Pos().x = i * -quads[i]->GetSize().x - interval * i + start;
		quads[i]->UpdateWorld();
	}

	UpdateWorld();
}

void ImageFont::Render()
{
	for (UINT i = 0; i < strValue.size(); ++i)
	{
		int num = (int)strValue[i] - (int)'0';
		int index = strValue.size() - i - 1;

		valueBuffer->Get()[0] = num;
		valueBuffer->SetPS(1);

		quads[index]->Render();
	}
}
