#include "Framework.h"
#include "GiganticEntMelee.h"
#include "Enemy.h"
#include "ProjectObjects/Character/Stat.h"
#include "ProjectObjects/Objects/AnimObject.h"


GiganticEntMelee::GiganticEntMelee(Enemy* owner, int effectFrame, float coolDown, float range)
	:EnemyAttack(owner, coolDown), effectFrame(effectFrame)
{
	rangeCollider = new CircleCollider(range);
	rangeCollider->SetParent(owner);
	rangeCollider->Pos().y += owner->GetDefaultSize().y * 0.5f;
	rangeCollider->SetTag("AttackRange");

	stamp = new AnimObject();
	stamp->AddClip("GiganticEnt_Stamp", L"ProjectTextures/Chapter1_GiganticEnt/Effect_Stamp", false, 1.2f);
	stamp->SetTag("GiganticEnt_Stamp");

	Clip* effectClip = stamp->FindClip("GiganticEnt_Stamp");
	Vector2 size = effectClip->GetFrame(8)->GetSize();

	data = { "Attack_Melee", 0.0f, 0.0f, 10, AttackType::Melee, size, Vector2(0.0f, size.y * 0.1f)};
	
	hitbox = new RectCollider(Vector2(size.x, size.y * 0.2f));
	hitbox->SetParent(stamp);
	hitbox->Pos() = data.hitboxOffset;
	hitbox->SetTag("GiganticEnt_Melee");
	hitbox->SetActive(false);

	effectClip->SetEvent([this]() {hitbox->SetActive(true); }, hitStartFrame);
	effectClip->SetEvent([this]() { hitbox->SetActive(false); }, hitEndFrame);
	effectClip->SetEvent([this]() { Finish(); });
}

GiganticEntMelee::~GiganticEntMelee()
{
	delete hitbox;
	delete stamp;
}

void GiganticEntMelee::Start(Character* target)
{
	EnemyAttack::Start(target);
	alreadyHit = false;
	hitbox->SetActive(false);

	castPos = owner->GlobalPos();

	Clip* clip = owner->FindClip(data.clipName);
	clip->SetEvent([this]() { stamp->Play("GiganticEnt_Stamp", castPos); }, 4);
	clip->SetEvent([this]() { owner->OnEndAction(); });

	owner->OnStartAction();
	owner->PlayClip(data.clipName);

}

void GiganticEntMelee::Update()
{
	stamp->Update();

	if (alreadyHit || !hitbox->Active()) return;

	hitbox->UpdateWorld();
	if (!hitbox->IsCollision(target->GetCollider())) return;

	alreadyHit = true;
	target->OnHit(MakeHitData());
}

void GiganticEntMelee::Cancel()
{
	stamp->Stop();
	Finish();
}

void GiganticEntMelee::Render()
{
	EnemyAttack::Render();
	stamp->Render();

	if (hitbox->Active())
	{
		hitbox->UpdateWorld();
		hitbox->Render();
	}
}

void GiganticEntMelee::Finish()
{
	if (finished) return;

	hitbox->SetActive(false);
	stamp->Stop();
	if (owner->IsPlayingAction())
		owner->OnEndAction();
	finished = true;
}

HitData GiganticEntMelee::MakeHitData()
{
	Stat* stat = owner->GetStatComponent();
	float damage = stat->CalculateFinalAttack((float)data.damage, DamageType::Physical, data.type, AttackActionType::Basic);

	return { damage, stat->GetStat(Attribute::CriticalChance), stat->GetStat(Attribute::CriticalDamage),
		DamageType::Physical, (target->GlobalPos().x - owner->GlobalPos().x) > 0 ? 1.0f : -1.0f, this, launchHeight };

}
