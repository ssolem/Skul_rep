#include "Framework.h"
#include "ThiefGoldAttack.h"
#include "ProjectObjects/Character/Character.h"
#include "../ThiefSkul.h"
#include "ProjectObjects/Character/Stat.h"
#include "ProjectObjects/Stage.h"

//GoldExplosion - AttackA 모션
ThiefGoldAttack::ThiefGoldAttack(Character* owner, ThiefSkul* thiefSkul)
	: Skill(owner, 0), thiefSkul(thiefSkul)
{
	hitbox = new RectCollider(Vector2::One);
	hitbox->SetActive(false);
	clipName = "AttackA";
}

ThiefGoldAttack::~ThiefGoldAttack()
{
	delete hitbox;
}

void ThiefGoldAttack::Init()
{
	TierData data;
	data = { 12.0f, 6.0f, 3, DamageType::Magic, 3 };
	tierDatas.push_back(data);
	data = { 12.0f, 6.0f, 3, DamageType::Magic };
	tierDatas.push_back(data);
	data = { 15.0f, 6.0f, 5, DamageType::Magic };
	tierDatas.push_back(data);
	data = { 18.0f, 6.0f, 10, DamageType::Magic };
	tierDatas.push_back(data);

	curData = tierDatas[thiefSkul->GetTier()];

	CreateGoldProjectile();

	SetCoolDown(curData.coolDown);
	coolDownTime = 0.0f;
}

void ThiefGoldAttack::Update()
{
}

void ThiefGoldAttack::UpdateCoolDown()
{
	if (coolDownTime > 0.0f)
		coolDownTime -= DELTA;

	//모션이 끝나도 남은 금화는 계속 발사
	if (remainShots <= 0) return;

	fireTimer -= DELTA;
	if (fireTimer > 0.0f) return;

	FireGoldProjectile();
	fireTimer += fireInterval;
}

void ThiefGoldAttack::Render()
{
}

void ThiefGoldAttack::Start()
{
	coolDownTime = coolDownMax;
	alreadyHit.clear();

	fireDir = owner->FacingRight() ? 1.0f : -1.0f;
	remainShots = curData.cost;
	fireTimer = 0.0f;		
}

void ThiefGoldAttack::End()
{
}

bool ThiefGoldAttack::IsReady()
{
	return coolDownTime <= 0.0f;
}

void ThiefGoldAttack::SetIcon()
{
	icon = Texture::Add(L"ProjectTextures/Thief/UI_Skill_Icons/GoldExplosion.png");
}

void ThiefGoldAttack::ApplyTier(int newTier)
{
	curData = tierDatas[newTier];

	SetCoolDown(curData.coolDown);
	coolDownTime = min(coolDownTime, coolDownMax);
}

HitData ThiefGoldAttack::CreateHitData(float attackDir)
{
	Stat* stat = owner->GetStatComponent();
	float damage = stat->CalculateFinalAttack(curData.damage, curData.damageType, curData.attackType, AttackActionType::Skill);

	return { damage, stat->GetStat(Attribute::CriticalChance), stat->GetStat(Attribute::CriticalDamage),
	curData.damageType, attackDir, this };
}

void ThiefGoldAttack::CreateGoldProjectile()
{
	ProjectileData data;
	data.tag = goldTag;
	if (ProjectileManager::Get()->HasData(data.tag)) return;

	data.path = L"ProjectTextures/Thief/Projectile";
	data.flyClip = "Thief_GoldExplosion_Projectile";
	data.hitClip = "";
	data.lifeTime = 5.0f;
	data.size = Vector2::Zero;
	data.speed = goldSpeed;
	data.tileHitType = TileHitType::Destroy;
	data.maxHitCount = 1;

	ProjectileManager::Get()->Register(data.tag, data);
}

void ThiefGoldAttack::FireGoldProjectile()
{
	//높이  랜덤
	float offsetY = Random(-fireOffsetY, fireOffsetY);
	Vector2 pos = owner->GlobalPos() + Vector2(0.0f, owner->GetDefaultSize().y * 0.5f + offsetY);
	Vector2 dir = { fireDir, 0.0f };

	if (curData.penetrateCount != 0)
		ProjectileManager::Get()->Fire(goldTag, pos, dir, CreateHitData(0.0f), ProjectileTarget::Enemy,
			0, nullptr, curData.penetrateCount);
	else
		ProjectileManager::Get()->Fire(goldTag, pos, dir, CreateHitData(0.0f), ProjectileTarget::Enemy,
			0, [this](Character* target) { GoldExplode(target->GetHitPos()); });

	remainShots--;
}

void ThiefGoldAttack::GoldExplode(Vector2 pos)
{
	hitbox->Scale() = Vector2(130.0f, 137.0f);
	hitbox->Pos() = pos;
	hitbox->UpdateWorld();
	hitbox->SetActive(true);

	HitData hitData = CreateHitData(0.0f);
	for (IHittable* hit : owner->GetStage()->CheckAllHittableCollision(hitbox))
	{
		hitData.attackDir = pos.x < hit->GetHitPos().x ? 1.0f : -1.0f;
		hit->OnHit(hitData);
	}

	EffectManager::Get()->Play("ThiefGoldExplosion", pos);
	hitbox->SetActive(false);
}
