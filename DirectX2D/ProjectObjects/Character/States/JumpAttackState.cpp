#include "Framework.h"
#include "JumpAttackState.h"
#include "../../Stage.h"
#include "../Stat.h"

JumpAttackState::JumpAttackState(Character* character, ActionType type, const AttackData& data, DamageType damageType, const vector<string>& clipNames)
	: ActionState(character, type, clipNames), data(data), damageType(damageType)
{
	isAttacking = false;
	attackCollider = character->GetAttackCollider();
	attackCollider->Pos() = { 0, 0 };
}

JumpAttackState::~JumpAttackState()
{
}

void JumpAttackState::ResetData(const AttackData& data, DamageType damageType)
{
	this->data = data;
	this->damageType = damageType;
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

	HitData hitData;
	bool hasHitData = false;

	for (IHittable* hit : owner->GetStage()->CheckAllHittableCollision(attackCollider))
	{
		if (find(alreadyHit.begin(), alreadyHit.end(), hit) != alreadyHit.end())
			continue;

		//새로 맞힌 대상이 처음 나왔을 때만 스탯 계산

		if (!hasHitData)
		{
			Stat* stat = owner->GetStatComponent();
			hitData = { stat->CalculateFinalAttack(data.damage, damageType, data.type, AttackActionType::Basic),
				stat->GetStat(Attribute::CriticalChance), stat->GetStat(Attribute::CriticalDamage),
				damageType, owner->FacingRight() ? 1.0f : -1.0f, this };
			hasHitData = true;
		}

		alreadyHit.push_back(hit);
		hit->OnHit(hitData);
	}
}
