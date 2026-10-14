#include "Framework.h"
#include "RootEntAttack.h"
#include "Enemy.h"
#include "ProjectObjects/Character/Stat.h"
#include "ProjectObjects/Objects/AnimObject.h"
#include "ProjectObjects/Stage.h"

RootEntAttack::RootEntAttack(Enemy* owner, int spikeFrame, float coolDown, float range)
	: EnemyAttack(owner, coolDown), spikeFrame(spikeFrame)
{
	data = { "Attack", 0.0f, 0.0f, 10, AttackType::Range, Vector2(124.0f, 58.0f), Vector2::Zero };

	rangeCollider = new CircleCollider(range);
	rangeCollider->SetParent(owner);
	rangeCollider->Pos().y += owner->GetDefaultSize().y * 0.5f;
	rangeCollider->SetTag("AttackRange");

	sign = new AnimObject();
	sign->AddClip("RootEnt_AttackSign", L"ProjectTextures/Chapter1_RootEnt/Effect_AttackSign");
	sign->SetTag("RootEnt_AttackSign");
	signOffset = Vector2(0.0f, -(sign->FindClip("RootEnt_AttackSign")->GetFrame(0)->GetSize().y * 0.5f));

	spike = new AnimObject();
	spike->AddClip("RootEnt_Attack", L"ProjectTextures/Chapter1_RootEnt/Effect_Attack");
	spike->FindClip("RootEnt_Attack")->SetFrameDelay(hitEndFrame, 0.8f);
	spike->SetTag("RootEntSpike");

	hitbox = new RectCollider(data.hitboxSize);
	hitbox->SetParent(spike);
	hitbox->Pos() = data.hitboxOffset;
	hitbox->Pos().y += data.hitboxSize.y * 0.5f;
	hitbox->SetTag("RootEntHitbox");
	hitbox->SetActive(false);

	sign->FindClip("RootEnt_AttackSign")->SetEvent([this]() { sign->Stop(); });

	Clip* spikeClip = spike->FindClip("RootEnt_Attack");
	spikeClip->SetEvent([this]() { hitbox->SetActive(true); }, hitStartFrame);
	spikeClip->SetEvent([this]() { hitbox->SetActive(false); }, hitEndFrame);
	spikeClip->SetEvent([this]() { Finish(); });
}

RootEntAttack::~RootEntAttack()
{
	delete hitbox;
	delete sign;
	delete spike;
}

void RootEntAttack::Start(Character* target)
{
	EnemyAttack::Start(target);
	alreadyHit = false;
	hitbox->SetActive(false);

	castPos = target->GetStage()->GetGroundPos(target->GlobalPos());

	Clip* clip = owner->FindClip(data.clipName);
	clip->SetEvent([this]() { sign->Play("RootEnt_AttackSign", castPos + signOffset); }, 0);
	clip->SetEvent([this]() { spike->Play("RootEnt_Attack", castPos); }, spikeFrame);
	clip->SetEvent([this]() { owner->OnEndAction(); });

	owner->OnStartAction();
	owner->PlayClip(data.clipName);
}

void RootEntAttack::Update()
{
	sign->Update();
	spike->Update();

	if (alreadyHit || !hitbox->Active()) return;

	hitbox->UpdateWorld();
	if (!hitbox->IsCollision(target->GetCollider())) return;

	alreadyHit = true;
	target->OnHit(MakeHitData());
}

//맞아서 취소되면 예고/뿌리 모두 거둠
void RootEntAttack::Cancel()
{
	sign->Stop();
	spike->Stop();
	Finish();
}

void RootEntAttack::Render()
{
	EnemyAttack::Render();
	sign->Render();
	spike->Render();

	if (hitbox->Active())
	{
		hitbox->UpdateWorld();
		hitbox->Render();
	}
}

void RootEntAttack::Finish()
{
	if (finished) return;

	hitbox->SetActive(false);
	spike->Stop();
	if (owner->IsPlayingAction())
		owner->OnEndAction();
	finished = true;
}

HitData RootEntAttack::MakeHitData()
{
	Stat* stat = owner->GetStatComponent();
	float damage = stat->CalculateFinalAttack((float)data.damage, DamageType::Physical, data.type, AttackActionType::Basic);

	return { damage, stat->GetStat(Attribute::CriticalChance), stat->GetStat(Attribute::CriticalDamage),
		DamageType::Physical, owner->FacingRight() ? 1.0f : -1.0f, this };
}
