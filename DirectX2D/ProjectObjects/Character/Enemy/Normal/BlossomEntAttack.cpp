#include "Framework.h"
#include "BlossomEntAttack.h"
#include "Enemy.h"
#include "ProjectObjects/Character/Stat.h"
#include "ProjectObjects/Objects/AnimObject.h"

BlossomEntAttack::BlossomEntAttack(Enemy* owner, float coolDown, float range)
	:EnemyAttack(owner, coolDown)
{
	rangeCollider = new CircleCollider(range);
	rangeCollider->SetParent(owner);
	rangeCollider->Pos().y += owner->GetDefaultSize().y * 0.5f;
	rangeCollider->SetTag("AttackRange");

	blossom = new AnimObject();
	blossom->AddClip("Effect_Attack_Fog", L"ProjectTextures/Chapter1_BlossomEnt/Atlas", false, 1.6f);
	blossom->SetTag("Effect_Attack_Fog");

	Clip* effectClip = blossom->FindClip("Effect_Attack_Fog");
	Vector2 size = effectClip->GetFrame(8)->GetSize();

	data = { "Attack_Ready", 0.0f, 0.0f, 5, AttackType::Melee, size, Vector2(0.0f, size.y * 0.5f) };

	hitbox = new RectCollider(size);
	hitbox->SetParent(blossom);
	hitbox->Pos() = data.hitboxOffset;
	hitbox->SetTag("BlossomEntHitbox");
	hitbox->SetActive(false);

	effectClip->SetEvent([this]() { hitbox->SetActive(true); }, 2);
	effectClip->SetEvent([this]() { hitbox->SetActive(false); }, 23);
	effectClip->SetEvent([this]() { Finish(); });

	Clip* clip = owner->FindClip(data.clipName);
	clip->SetEvent([this, owner]() 
		{
			owner->PlayClip("Attack");
			blossom->Play("Effect_Attack_Fog", castPos);
		});
}

BlossomEntAttack::~BlossomEntAttack()
{
	delete hitbox;
	delete blossom;
}

void BlossomEntAttack::Start(Character* target)
{
	EnemyAttack::Start(target);
	alreadyHit = false;
	hitbox->SetActive(false);

	castPos = owner->GlobalPos();

	owner->OnStartAction();
	owner->PlayClip(data.clipName);
}

void BlossomEntAttack::Update()
{
	blossom->Update();

	if (alreadyHit || !hitbox->Active()) return;

	hitbox->UpdateWorld();
	if (!hitbox->IsCollision(target->GetCollider())) return;

	alreadyHit = true;
	target->OnHit(MakeHitData());
}

void BlossomEntAttack::Cancel()
{
	blossom->Stop();
	Finish();
}

void BlossomEntAttack::Render()
{
	EnemyAttack::Render();
	blossom->Render();

	if (hitbox->Active())
	{
		hitbox->UpdateWorld();
		hitbox->Render();
	}
}

void BlossomEntAttack::Finish()
{
	if (finished) return;

	hitbox->SetActive(false);
	blossom->Stop();
	if (owner->IsPlayingAction())
		owner->OnEndAction();
	finished = true;
}

HitData BlossomEntAttack::MakeHitData()
{
	Stat* stat = owner->GetStatComponent();
	float damage = stat->CalculateFinalAttack((float)data.damage, DamageType::Physical, data.type, AttackActionType::Basic);

	return { damage, stat->GetStat(Attribute::CriticalChance), stat->GetStat(Attribute::CriticalDamage),
		DamageType::Physical, (target->GlobalPos().x - owner->GlobalPos().x) > 0 ? 1.0f : -1.0f, this };

}