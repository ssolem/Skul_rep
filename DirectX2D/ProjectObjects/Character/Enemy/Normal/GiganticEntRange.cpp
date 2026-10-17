#include "Framework.h"
#include "GiganticEntRange.h"
#include "ProjectObjects/Character/Animator.h"
#include "ProjectObjects/Character/Stat.h"
#include "Enemy.h"



GiganticEntRange::GiganticEntRange(Enemy* owner, int projectileFrame, float coolDown, float range)
	: EnemyAttack(owner, coolDown), projectileFrame(projectileFrame)
{
	CreateProjectile();

	rangeCollider = new CircleCollider(range);
	rangeCollider->SetParent(owner);
	rangeCollider->Pos().y += owner->GetDefaultSize().y * 0.5f;
	rangeCollider->SetTag("AttackRange");

	data = { "Attack_Range", 0.0f, 0.0f, 10, AttackType::Projectile, Vector2::Zero, Vector2::Zero };

	hitbox = nullptr;
}

GiganticEntRange::~GiganticEntRange()
{
	delete hitbox;
}

void GiganticEntRange::Start(Character* target)
{
	EnemyAttack::Start(target);

	castPos = owner->GlobalPos() + Vector2(0.0f, owner->GetDefaultSize().y * 0.5f);

	Clip* clip = owner->FindClip(data.clipName);
	clip->SetEvent([this]() { Fire(); }, projectileFrame);
	clip->SetEvent([this]() { Finish(); });
	
	owner->OnStartAction();
	owner->PlayClip(data.clipName);
}

void GiganticEntRange::Update()
{
}

void GiganticEntRange::Cancel()
{
	Finish();
}

void GiganticEntRange::Render()
{
	EnemyAttack::Render();
}

void GiganticEntRange::Finish()
{
	if (finished) return;

	if (owner->IsPlayingAction())
		owner->OnEndAction();

	finished = true;
}

HitData GiganticEntRange::MakeHitData()
{
	Stat* stat = owner->GetStatComponent();
	float damage = stat->CalculateFinalAttack((float)data.damage, DamageType::Physical, data.type, AttackActionType::Basic);

	return { damage, stat->GetStat(Attribute::CriticalChance), stat->GetStat(Attribute::CriticalDamage),
		DamageType::Physical, 0.0f, this };

}

void GiganticEntRange::CreateProjectile()
{
	ProjectileData data;
	data.tag = "GiantEntBullet";
	if (ProjectileManager::Get()->HasData(data.tag)) return;
	data.path = L"ProjectTextures/Chapter1_GiganticEnt/Atlas";
	data.lifeTime = 5.0f;
	data.size = Vector2::Zero;
	data.speed = 200.0f;
	data.tileHitType = TileHitType::Pass;
	data.pierce = false;

	ProjectileManager::Get()->Register(data.tag, data);
}

void GiganticEntRange::Fire()
{
	//한 번 발사한 총알 전체를 한 그룹으로 묶어 한 발만 맞게 함

	unsigned int groupId = ProjectileManager::Get()->NewGroupId();

	float startAngle = XM_PI / 2;
	float step = XM_2PI / projectileCount;
	for (int i = 0; i < projectileCount; i++)
	{
		float angle = startAngle + step * i;
		Vector2 dir = { cos(angle), sin(angle) };
		ProjectileManager::Get()->Fire("GiantEntBullet", castPos, dir, MakeHitData(), ProjectileTarget::Player, groupId);
	}
}
