#include "Framework.h"

Button::Button(wstring textureFile)
	: Quad(textureFile)
{
	collider = new RectCollider(size);
	collider->SetParent(this);
}

Button::Button(Vector2 size)
	: Quad(size)
{
	collider = new RectCollider(size);
	collider->SetParent(this);
}

Button::~Button()
{
	delete collider;
}

void Button::Update()
{
	if (!isActive) return;

	// 편의상 여기서 조작을 받자 (-> 함수를 외부에 공개시키고, 컨트롤러에 등록)

	if (collider->IsPointCollision(mousePos))
	{
		// 마우스가 버튼 위에 올라와 있는 상태

		if (KEY_DOWN(VK_LBUTTON))
			isDownCheck = true;

		if (KEY_PRESS(VK_LBUTTON))
			state = State::DOWN;
		else
			state = State::OVER;

		if (isDownCheck && KEY_UP(VK_LBUTTON)) // 버튼 놓기
		{
			// 버튼을 놓으면서 동작이 시작되는 경우라면 여기서

			if (event != nullptr) event(); // 이벤트가 있었으면 호출
			if (paramEvent != nullptr) paramEvent(object); // 인수 이벤트도 있으면 호출

			isDownCheck = false;
		}
	}
	else
	{
		state = State::NONE;

		if (KEY_UP(VK_LBUTTON)) isDownCheck = false;
	}

	// 색 변경
	switch (state)
	{
	case Button::State::NONE:
		colorBuffer->Get() = NONE_COLOR;
		break;
	case Button::State::DOWN:
		colorBuffer->Get() = DOWN_COLOR;
		break;
	case Button::State::OVER:
		colorBuffer->Get() = OVER_COLOR;
		break;
	}

	UpdateWorld();
	collider->UpdateWorld();
}

void Button::Render()
{
	if (!isActive) return;

	Quad::Render();
	collider->Render();
}