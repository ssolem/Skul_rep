#include "Framework.h"
#include "ObjectController.h"

ObjectController::~ObjectController()
{
	for (pair<string, InputAction*> keyMap : keyMaps)
	{
		delete keyMap.second;
	}

	for (pair<string, ParamEvent> event : events)
	{
		event.second = nullptr;
	}
}

void ObjectController::AddInput(string inputName, UINT key, InputAction::InputType triggerType, void* value)
{
	if (!target) return;

	keyMaps.insert(
		pair<string, InputAction*>
		(inputName, new InputAction(key, triggerType, value))
	);
}

void ObjectController::BindAction(string inputName, ParamEvent action)
{
	if (!target) return;
	events[inputName] = action;
}

InputAction* ObjectController::GetInput(string actionName)
{
	if (keyMaps.count(actionName) < 1) return nullptr;
	return keyMaps[actionName];
}

void ObjectController::Update()
{
	if (!target || events.empty()) return;
	
	for (pair<string, ParamEvent> event : events)
	{
		if (keyMaps[event.first]->InputApplied())
		{
			event.second(keyMaps[event.first]->GetValue());
		}
	}
}
