#include "Framework.h"
#include "MeleeSwing.h"
#include "Enemy.h"
#include "ProjectObjects/Character/Stat.h"

MeleeSwing::MeleeSwing(Enemy* owner, const AttackData& data, int hitFrame, float coolDown)
	: EnemyAttack(owner, coolDown), data(data), hitFrame(hitFrame)
{
	hitbox = owner->GetAttackCollider();

	//공격 가능 범위 = Start에서 히트박스를 놓는 것과 같은 크기/위치
	rangeCollider = new RectCollider(data.hitboxSize);
	rangeCollider->SetParent(owner);
	rangeCollider->Pos() = data.hitboxOffset;
	rangeCollider->Pos().y += data.hitboxSize.y * 0.5f;
	rangeCollider->SetTag("AttackRange");
}

void MeleeSwing::Start(Character* target)
{
	EnemyAttack::Start(target);
	alreadyHit = false;

	((RectCollider*)hitbox)->SetSize(data.hitboxSize);
	hitbox->Pos() = data.hitboxOffset;
	hitbox->Pos().y += data.hitboxSize.y * 0.5f;
	hitbox->SetActive(false);		//hitFrame에 켜짐

	Clip* clip = owner->FindClip(data.clipName);
	clip->SetEvent([this]() { hitbox->SetActive(true); }, hitFrame);
	clip->SetEvent([this]() { Finish(); });

	owner->OnStartAction();
	owner->PlayClip(data.clipName);
}

void MeleeSwing::Update()
{
	if (alreadyHit || !hitbox->Active()) return;

	hitbox->UpdateWorld();
	if (!hitbox->IsCollision(target->GetCollider())) return;

	alreadyHit = true;		//한 번 휘두를 때 한 번만 타격
	target->OnHit(MakeHitData());
}

void MeleeSwing::Cancel()
{
	Finish();
}

void MeleeSwing::Render()
{
	EnemyAttack::Render();
	hitbox->Render();		//비활성이면 Collider::Render가 알아서 건너뜀
}

void MeleeSwing::Finish()
{
	if (finished) return;

	hitbox->SetActive(false);
	hitbox->Pos() = Vector2::Zero;
	owner->OnEndAction();
	finished = true;
}

HitData MeleeSwing::MakeHitData()
{
	Stat* stat = owner->GetStatComponent();
	float damage = stat->CalculateFinalAttack(data.damage, DamageType::Physical, data.type, AttackActionType::Basic);

	return { damage, stat->GetStat(Attribute::CriticalChance), stat->GetStat(Attribute::CriticalDamage),
		DamageType::Physical, owner->FacingRight() ? 1.0f : -1.0f, this };
}
