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
	if (curSkill == nullptr) return;

	isAttacking = true;
	curClipName = curSkill->GetClipName();

	//순간 발동 스킬이 Start 안에서 바로 Finish()를 불러도 되도록 Start보다 먼저 연결
	curSkill->SetOnFinish([this]() { End(); });

	if (!curClipName.empty())
	{
		Clip* curClip = owner->FindClip(curClipName);
		assert(curClip != nullptr && "스킬 clipName 확인");

		curClip->SetEvent([this]() { End(); });
		PlayClip();
	}

	curSkill->Start();
}

void SkillState::End()
{
	if (!isAttacking) return;

	isAttacking = false;
	owner->OnEndAction();

	if (curSkill != nullptr) curSkill->End();
}

bool SkillState::IsReady()
{
	if (curSkill != nullptr)
		return curSkill->IsReady();

	return false;
}
