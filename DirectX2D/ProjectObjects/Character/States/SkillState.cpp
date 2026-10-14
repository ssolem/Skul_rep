#include "Framework.h"
#include "SkillState.h"
#include "ProjectObjects/Character/Skill.h"

SkillState::SkillState(Character* character, ActionType type, const vector<string>& clipNames)
	: ActionState(character, type, clipNames), isAttacking(false), curSkill(nullptr)
{
	isAttacking = false;
}

SkillState::~SkillState()
{
}

void SkillState::Update()
{
	if (curSkill != nullptr) curSkill->Update();
}

void SkillState::UpdateCoolDown()
{
	if (curSkill != nullptr) curSkill->UpdateCoolDown();
}


void SkillState::Render()
{
	if (curSkill != nullptr) curSkill->Render();
}

void SkillState::Start()
{
    isAttacking = true;

    Clip* curClip = owner->FindClip(curClipName);

    if (curClip != nullptr)
    {
        curClip->SetEvent([this]() { End(); });
        PlayClip();

        if (curSkill != nullptr) curSkill->Start();
    }
    else
    {
        if (curSkill != nullptr) curSkill->Start();
        End();
    }
}

void SkillState::End()
{
	owner->OnEndAction();
	isAttacking = false;

	if (curSkill != nullptr) curSkill->End();
}

bool SkillState::IsReady()
{
	if (curSkill != nullptr)
		return curSkill->IsReady();
}
