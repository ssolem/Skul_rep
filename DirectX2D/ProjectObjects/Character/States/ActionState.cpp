#include "Framework.h"
#include "ActionState.h"

ActionState::ActionState(Character* character, ActionType type, const vector<string>& clipNames)
	: owner(character), type(type), clipNames(clipNames)
{
	curClipName = clipNames[0];
}

ActionState::~ActionState()
{
}

void ActionState::Update()
{
}

void ActionState::UpdateCoolDown()
{
}

void ActionState::Render()
{
}

void ActionState::PlayClip()
{
	if (owner != nullptr)
		owner->PlayClip(curClipName);
}

void ActionState::Start()
{
}

void ActionState::End()
{
}

void ActionState::ResetActions(const vector<string>& clips)
{
	clipNames = clips;
	curClipName = clipNames.empty() ? "" : clipNames[0];
}
