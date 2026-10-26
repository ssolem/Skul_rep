#include "Framework.h"
#include "ThiefSkulSwitch.h"
#include "ProjectObjects/Character/Character.h"
#include "ProjectObjects/Character/Stat.h"
#include "ProjectObjects/Stage.h"

ThiefSkulSwitch::ThiefSkulSwitch(Character* owner)
	: Skill(owner, 0)
{
	
}

ThiefSkulSwitch::~ThiefSkulSwitch()
{
}

void ThiefSkulSwitch::Init()
{
}

void ThiefSkulSwitch::Update()
{
	Move();
	CheckHit();
}

void ThiefSkulSwitch::Render()
{
	if (attackCollider->Active())
		attackCollider->Render();
}

void ThiefSkulSwitch::Start()
{
	Clip* clip = owner->FindClip(slashClip);
	clip->ClearEvents();	
	clip->SetEvent([this]() { Finish(); });

	backDir = owner->FacingRight() ? -1.0f : 1.0f;
	moveSpeed = MOVE_DISTANCE / clip->GetDuration();
	movedDistance = 0.0f;

	alreadyHit.clear();

	attackCollider = owner->GetAttackCollider();
	Vector2 size = clip->GetFrame(HITBOX_FRAME)->GetSize();
	((RectCollider*)attackCollider)->SetSize(size);
	attackCollider->Pos() = { 0.0f, size.y * 0.5f };
	attackCollider->SetActive(true);

	owner->PlayClip(slashClip);
}

void ThiefSkulSwitch::End()
{
	attackCollider->SetActive(false);
	attackCollider->Pos() = { 0, 0 };
}

void ThiefSkulSwitch::Move()
{
	float moveAmount = moveSpeed * DELTA;
	if (movedDistance + moveAmount > MOVE_DISTANCE)
		moveAmount = MOVE_DISTANCE - movedDistance;

	owner->Pos().x += backDir * moveAmount;
	movedDistance += moveAmount;
}

void ThiefSkulSwitch::CheckHit()
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
			hitData = { stat->CalculateFinalAttack(damage, DamageType::Magic, AttackType::Melee, AttackActionType::Skill),
				stat->GetStat(Attribute::CriticalChance), stat->GetStat(Attribute::CriticalDamage),
				DamageType::Magic, owner->FacingRight() ? 1.0f : -1.0f, this };
			hasHitData = true;
		}

		alreadyHit.push_back(hit);
		hit->OnHit(hitData);
	}
}
