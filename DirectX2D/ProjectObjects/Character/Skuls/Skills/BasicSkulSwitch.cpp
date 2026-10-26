#include "Framework.h"
#include "BasicSkulSwitch.h"
#include "ProjectObjects/Character/Character.h"
#include "ProjectObjects/Character/Stat.h"
#include "ProjectObjects/Stage.h"

BasicSkulSwitch::BasicSkulSwitch(Character* owner)
	: Skill(owner, 0)
{
}

BasicSkulSwitch::~BasicSkulSwitch()
{
}

void BasicSkulSwitch::Init()
{
}

void BasicSkulSwitch::Update()
{
	timer += DELTA;
	halfTurnTimer += DELTA;

	//반 바퀴마다 같은 적을 다시 때릴 수 있게
	if (halfTurnTimer >= halfTurnTime)
	{
		halfTurnTimer -= halfTurnTime;
		alreadyHit.clear();
	}

	Move();
	CheckHit();

	if (timer >= totalTime)
		Finish();
}

void BasicSkulSwitch::Render()
{
	if (attackCollider->Active())
		attackCollider->Render();
}

void BasicSkulSwitch::Start()
{
	clip = owner->FindClip(spinClip);
	clip->ClearEvents();	
	clip->SetLoop(true);

	float spinTime = clip->GetDuration();
	totalTime = spinTime * SPIN_COUNT;
	halfTurnTime = spinTime * 0.5f;
	timer = 0.0f;
	halfTurnTimer = 0.0f;

	moveDir = owner->FacingRight() ? 1.0f : -1.0f;
	moveSpeed = MOVE_DISTANCE / totalTime;

	alreadyHit.clear();

	attackCollider = owner->GetAttackCollider();
	Vector2 size = clip->GetFrame(HITBOX_FRAME)->GetSize();
	((RectCollider*)attackCollider)->SetSize(size);
	attackCollider->Pos() = { 0.0f, size.y * 0.5f };	
	attackCollider->SetActive(true);

	owner->PlayClip(spinClip);
}

void BasicSkulSwitch::End()
{
	attackCollider->SetActive(false);
	attackCollider->Pos() = { 0, 0 };

	if (clip) clip->SetLoop(false);
}

void BasicSkulSwitch::Move()
{
	owner->Pos().x += moveDir * moveSpeed * DELTA;
}

void BasicSkulSwitch::CheckHit()
{
	attackCollider->UpdateWorld();

	HitData hitData;
	bool hasHitData = false;

	for (IHittable* hit : owner->GetStage()->CheckAllHittableCollision(attackCollider))
	{
		if (find(alreadyHit.begin(), alreadyHit.end(), hit) != alreadyHit.end())
			continue;

		if (!hasHitData)
		{
			Stat* stat = owner->GetStatComponent();
			hitData = { stat->CalculateFinalAttack(damage, DamageType::Physical, AttackType::Melee, AttackActionType::Skill),
				stat->GetStat(Attribute::CriticalChance), stat->GetStat(Attribute::CriticalDamage),
				DamageType::Physical, 0.0f, this };
			hasHitData = true;
		}

		hitData.attackDir = owner->GlobalPos().x < hit->GetHitPos().x ? 1.0f : -1.0f;

		alreadyHit.push_back(hit);
		hit->OnHit(hitData);
	}
}
