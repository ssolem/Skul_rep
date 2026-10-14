#include "Framework.h"

ButtonObject::ButtonObject(wstring textureFile)
	: Button(textureFile),	// 부모 데이터 생성
	key(textureFile)		// 기본 호출명을 그림파일 이름으로
{
	SetEvent(bind(&ButtonObject::OnClick, this));

	//외곽선 설정
	SetPixelShader(L"Outline.hlsl");
	intValueBuffer = new IntValueBuffer();
	floatValueBuffer = new FloatValueBuffer();
	outlineColorBuffer = new ColorBuffer();

	floatValueBuffer->Get()[0] = size.x;	
	floatValueBuffer->Get()[1] = size.y;	

	outlineColorBuffer->Get() = { 1, 0, 0, 1 };
}

ButtonObject::~ButtonObject()
{
	delete intValueBuffer;
	delete floatValueBuffer;
	delete outlineColorBuffer;
}

void ButtonObject::Update()
{
	// 따로 수행할 기능이나 애니메이션이 있으면 수행
	if (isSelected)
		intValueBuffer->Get()[0] = outlineScale;
	else
		intValueBuffer->Get()[0] = 0;


	Button::Update();
}

void ButtonObject::Render()
{
	if (!isActive) return;

	intValueBuffer->SetPS(1);
	floatValueBuffer->SetPS(2);
	outlineColorBuffer->SetPS(3);

	Button::Render();
}

void ButtonObject::SetNewOutlineColor(float r, float g, float b, float a)
{
	outlineColorBuffer->Get() = { r, g, b, a };
}

void ButtonObject::OnClick()
{
	// 감시 클래스 사용
	Observer::Get()->ExecuteParamEvent("ButtonAction", this);
								// 이 '키'에 해당하는 2차 함수가
								// 어디의 뭐냐가 관건
}
