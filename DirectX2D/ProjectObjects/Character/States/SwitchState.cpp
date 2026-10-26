#include "Framework.h"
#include "SwitchState.h"
#include "ProjectObjects/Character/Skill.h"

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
	if (isSwitching && switchSkill != nullptr) switchSkill->Update();
}

void SwitchState::UpdateCoolDown()
{
}


void SwitchState::Render()
{
	if (isSwitching && switchSkill != nullptr) switchSkill->Render();
}

void SwitchState::Start()
{
	isSwitching = true;

	//교대 스킬 없는 스컬 - 클립만 재생
	if (switchSkill == nullptr)
	{
		Clip* clip = owner->FindClip(curClipName);
		clip->ClearEvents();
		clip->SetEvent([this]() { End(); });
		PlayClip();
		return;
	}

	switchSkill->SetOnFinish([this]() { End(); });
	switchSkill->Start();
}

void SwitchState::End()
{
	if (!isSwitching) return;

	isSwitching = false;
	owner->OnEndAction();

	if (switchSkill != nullptr) switchSkill->End();
}

bool SwitchState::IsReady()
{
	return !isSwitching;
}
