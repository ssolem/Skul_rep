#include "Framework.h"
#include "SwitchState.h"

SwitchState::SwitchState(Character* character, ActionType type, const vector<string>& clipNames)
	: ActionState(character, type, clipNames)
{
	isSwitching = false;
}

SwitchState::~SwitchState()
{
}

void SwitchState::Update()
{
}

void SwitchState::UpdateCoolDown()
{
}


void SwitchState::Render()
{
}

void SwitchState::Start()
{
	isSwitching = true;

	Clip* clip = owner->FindClip(curClipName);
	clip->ClearEvents();
	clip->SetEvent([this]() { End(); });
	PlayClip();
}

void SwitchState::End()
{
	owner->OnEndAction();
	isSwitching = false;
}

bool SwitchState::IsReady()
{
	return !isSwitching;
}
