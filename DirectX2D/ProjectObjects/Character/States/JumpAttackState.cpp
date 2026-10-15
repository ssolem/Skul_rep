#include "Framework.h"
#include "JumpAttackState.h"
#include "../../Stage.h"
#include "../Stat.h"

JumpAttackState::JumpAttackState(Character* character, ActionType type, const AttackData& data, const vector<string>& clipNames)
	: ActionState(character, type, clipNames), data(data)
{
	isAttacking = false;
	attackCollider = character->GetAttackCollider();
	attackCollider->Pos() = { 0, 0 };
}

JumpAttackState::~JumpAttackState()
{
}

void JumpAttackState::Update()
{
	CheckHit();
}

void JumpAttackState::UpdateCoolDown()
{
}

void JumpAttackState::Render()
{
	if (attackCollider != nullptr)
		attackCollider->Render();
}

void JumpAttackState::Start()
{
	isAttacking = true;
	alreadyHit.clear();

	((RectCollider*)attackCollider)->SetSize(data.hitboxSize);
	attackCollider->Pos().y += data.hitboxSize.y * 0.5f;
	attackCollider->Pos() += data.hitboxOffset;
	attackCollider->SetActive(true);

	Clip* clip = owner->FindClip(curClipName);
	clip->SetEvent([this]() { owner->SetCancellable(true); }, 0);
	clip->SetEvent([this]() { End(); });
	PlayClip();
}

void JumpAttackState::End()
{
	attackCollider->SetActive(false);
	attackCollider->Pos() = { 0, 0 };

	owner->OnEndAction();
	isAttacking = false;
}

bool JumpAttackState::IsReady()
{
	return !(owner->IsGrounded()) && !isAttacking;
}

void JumpAttackState::CheckHit()
{
	attackCollider->UpdateWorld();

	owner->CheckFacing();

	float finalDamage = owner->GetStatComponent()->CalculateFinalAttack(data.damage, damageType, data.type, AttackActionType::Basic);
	float critChance = owner->GetStatComponent()->GetStat(Attribute::CriticalChance);
	float critDamage = owner->GetStatComponent()->GetStat(Attribute::CriticalDamage);

	HitData hitData = { finalDamage, critChance, critDamage, damageType, owner->FacingRight() ? 1.0f : -1.0f, this };

	for (IHittable* hit : owner->GetStage()->CheckAllHittableCollision(attackCollider))
	{
		if (find(alreadyHit.begin(), alreadyHit.end(), hit) != alreadyHit.end())
			continue; 

		alreadyHit.push_back(hit);
		hit->OnHit(hitData);
	}
}
