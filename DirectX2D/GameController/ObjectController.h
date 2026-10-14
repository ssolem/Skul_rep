#pragma once

#include "InputAction.h"

// 객체용 컨트롤러. 게임 혹은 오브젝트에서 사용 가능

class ObjectController
{
public:

	ObjectController(GameObject* owner) : target(owner) {}
	~ObjectController();

	// 키 지정하기, (키에 따른) 입력 결과(이벤트) 연결하기
	void AddInput(string inputName,
		UINT key, InputAction::InputType triggerType, void* value);

	void BindAction(string inputName, ParamEvent action);


	// 등록된 입력 액션 참조하기 (참조 및 조작에 모두 필요)
	InputAction* GetInput(string actionName);

	void Update();

private:

	GameObject* target; // 조작의 대상. 예를 들면, 플레이어

	unordered_map<string, InputAction*> keyMaps;
	unordered_map<string, ParamEvent> events;
};

