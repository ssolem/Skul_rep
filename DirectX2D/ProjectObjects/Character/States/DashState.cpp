#include "Framework.h"
#include "DashState.h"


DashState::DashState(Character* character, ActionType type, const vector<string>& clipNames)
	: ActionState(character, type, clipNames)
{
}

DashState::~DashState()
{
}

void DashState::Update()
{
	float moveAmount = dashSpeed * DELTA;

	if (dashMovedDistance + moveAmount >= dashDistance)
	{
		moveAmount = dashDistance - dashMovedDistance;
		End();
	}

	owner->Pos().x += dashDirection.x * moveAmount;
	dashMovedDistance += moveAmount;
}

void DashState::UpdateCoolDown()
{
	SetNextDirection(GetInputDirX());

	if (isDashing) return; 
	if (dashCoolDown <= 0.0f) return;

	dashCoolDown -= DELTA;
	if (dashCoolDown <= 0.0f)
	{
		dashCoolDown = 0.0f;
		dashCount = 0; 
	}
}

void DashState::Render()
{
}

void DashState::Start()
{
	dashMovedDistance = 0.0f;

	float dirX = hasNextDir ? nextDirX : owner->GetDirection().x;

	if (dirX > 0.0f) dashDirection = { 1.0f, 0.0f };
	else if(dirX < 0.0f) dashDirection = { -1.0f, 0.0f };
	else dashDirection = { owner->FacingRight() ? 1.0f : -1.0f, 0.0f };

	hasNextDir = false;
	owner->SetFacing(dashDirection.x > 0.0f);

	dashCount++;
	isDashing = true;

	if (dashCount >= maxDashCount)
		dashCoolDown = dashCoolDownMax;

	dashEffectPos = owner->GlobalPos() - (dashDirection * dashEffectLocationOffset);
	EffectManager::Get()->Play("DashSmoke_Public_Small", dashEffectPos, owner->FacingRight());
	owner->SetVelocity(Vector2::Zero);

	Clip* clip = owner->FindClip(curClipName);
	if (clip != nullptr)
	{
		clip->SetSpeed(1.0f);
		float clipDuration = clip->GetDuration();
		float dashDuration = dashDistance / dashSpeed;
		clip->SetSpeed(clipDuration / dashDuration);
	}

	PlayClip();
}

void DashState::End()
{
	isDashing = false;
	owner->OnEndAction();
}

bool DashState::IsReady()
{
	return dashCount < maxDashCount;
}
