#pragma once

class Button : public Quad
{
protected:
	enum class State
	{
		NONE,	// 안 눌렀음
		DOWN,	// 눌렀음
		OVER,	// 안 눌렀는데, 커서는 위에 있음
		// RELEASE, // 눌렀는데, 지금 손 뗐음
	};

	// 각 상태에 따른 색변화
	const Float4 NONE_COLOR = { 1.0f, 1.0f, 1.0f, 1.0f };
	const Float4 DOWN_COLOR = { 0.5f, 0.5f, 0.5f, 1.0f };
	const Float4 OVER_COLOR = { 0.9f, 0.9f, 0.9f, 1.0f };

public:
	Button(wstring textureFile);
	Button(Vector2 size);
	~Button();

	void Update();
	void Render();

	void SetEvent(Event event) { this->event = event; }
	// void SetEvent(ParamEvent event) { this->paramEvent = event; }// 이래도 됨
	void SetParamEvent(ParamEvent event) { this->paramEvent = event; }
	void SetObject(void* object) { this->object = object; }

	RectCollider* GetCollider() { return collider; }

private:

	RectCollider* collider;		// 버튼을 누르는 과정에 대한 데이터
	State state = State::NONE;
	bool isDownCheck = false;

	Event event = nullptr;		// 단독 기능이 있다면 이 이름으로 등록
	ParamEvent paramEvent = nullptr; // 인수 있는 함수 포인터면 여기 등록
	void* object;						// 대상이 정해질 수 있다면 여기 등록
};

