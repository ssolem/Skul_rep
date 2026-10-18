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
	clipName = "AttackA";
}

ThiefGoldAttack::~ThiefGoldAttack()
{
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
}

void ThiefGoldAttack::Render()
{
}

void ThiefGoldAttack::Start()
{
	coolDownTime = coolDownMax;
	alreadyHit.clear();

	//TODO: 판정/금화 폭발 처리 (종료는 SkillState가 AttackA 마지막 프레임에서 처리)
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

	//쿨다운은 이어서 받음
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
	data.maxHitCount = 3;

	ProjectileManager::Get()->Register(data.tag, data);
}

void ThiefGoldAttack::FireGoldProjectile()
{
	HitData hitData = CreateHitData(0.0f);
	Vector2 pos = owner->GlobalPos() + Vector2(0.0f, owner->GetDefaultSize().y * 0.5f);

	for (int i = 0; i < curData.cost; i++)
	{
		float angle = XMConvertToRadians(30.0f + 20.0f * i);
		Vector2 dir = { owner->FacingRight()? 1.0f : -1.0f, 0.0f};
		ProjectileManager::Get()->Fire(goldTag, pos, dir, hitData, ProjectileTarget::Enemy);
	}
}
