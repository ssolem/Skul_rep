#include "Framework.h"
#include "JumpAttackState.h"

JumpAttackState::JumpAttackState(Character* character, ActionType type, const vector<string>& clipNames)
	: ActionState(character, type, clipNames)
{
	isAttacking = false;
}

JumpAttackState::~JumpAttackState()
{
}

void JumpAttackState::Update()
{
}

void JumpAttackState::UpdateCoolDown()
{
}

void JumpAttackState::Render()
{
}

void JumpAttackState::Start()
{
	isAttacking = true;

	owner->FindClip(curClipName)->SetEvent([this]() {owner->SetCancellable(true); }, 0);
	owner->FindClip(curClipName)->SetEvent([this]() { End(); });
	PlayClip();
}

void JumpAttackState::End()
{
	owner->OnEndAction();
	isAttacking = false;
}

bool JumpAttackState::IsReady()
{
	return !(owner->IsGrounded()) && !isAttacking;
}
