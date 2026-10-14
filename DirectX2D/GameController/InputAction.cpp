#include "Framework.h"
#include "InputAction.h"

bool InputAction::InputApplied()
{
	if (type == InputType::DOWN)
	{
		return KEY_DOWN(key);
	}
	else if (type == InputType::UP)
	{
		return KEY_UP(key);
	}
	else if (type == InputType::PRESS)
	{
		return KEY_PRESS(key);
	}
	else // NONE
	{
		bool result = (KEY_PRESS(key) || KEY_UP(key) || KEY_DOWN(key));
		return !result;
	}
}
