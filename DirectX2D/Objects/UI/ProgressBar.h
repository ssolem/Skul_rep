#pragma once

//진행 정도 표시 막대 : 체력 표시등에 활용

class ProgressBar : public Quad //스스로가 주 데이터
{
public:
	ProgressBar(wstring frontImageFile, wstring backImageFile);
	virtual ~ProgressBar();

	void Render();
	void SetAmount(float value);

private:
	//진행 표시기의 준비물
	//Quad* mainImage; // 주 표시 데이터

	Texture* backImage; // 배경 이미지
	FloatValueBuffer* valueBuffer; // 소수 버퍼 - 진행 정도를 표시하는데 활용
	float fillAmount = 1.0f; // 진행 정도를 나타내는 값 (0.0 ~ 1.0)
};

