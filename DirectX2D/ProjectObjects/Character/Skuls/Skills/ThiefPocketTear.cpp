#include "Framework.h"
#include "ThiefPocketTear.h"
#include "ProjectObjects/Character/Character.h"
#include "../ThiefSkul.h"
#include "ProjectObjects/Character/Stat.h"
#include "ProjectObjects/Stage.h"

ThiefPocketTear::ThiefPocketTear(Character* owner, ThiefSkul* thiefSkul)
	: Skill(owner, 0), thiefSkul(thiefSkul)
{
	hitbox = new RectCollider(Vector2::One);
	hitbox->SetActive(false);
}

ThiefPocketTear::~ThiefPocketTear()
{
	delete hitbox;
}

void ThiefPocketTear::Init()
{
	//struct TierData { damage, coolDown, goldPerHit, goldChance, additionalGoldHitTime, effectKey, slashSize }
	TierData data;
	data = { 15.0f, 8.0f, 4, 0.0f, 0.0f, "ThiefPocketTear",  Vector2(194.0f, 138.0f) };
	tierDatas.push_back(data);
	data = { 15.0f, 8.0f, 6, 0.0f, 0.0f, "ThiefPocketTear",  Vector2(194.0f, 138.0f) };
	tierDatas.push_back(data);
	data = { 20.0f, 8.0f, 8, 0.0f, 5.0f, "ThiefPocketTear",  Vector2(194.0f, 138.0f) };
	tierDatas.push_back(data);
	data = { 25.0f, 8.0f, 10, 0.01f, 5.0f, "Thief4PocketTear", Vector2(610.0f, 526.0f) };
	tierDatas.push_back(data);

	ApplyTier(thiefSkul->GetTier());
	coolDownTime = 0.0f;
}

void ThiefPocketTear::Update()
{
}

void ThiefPocketTear::UpdateCoolDown()
{
	if (coolDownTime > 0.0f)
		coolDownTime -= DELTA;
}

void ThiefPocketTear::Render()
{
	if (hitbox->Active())
		hitbox->Render();
}

void ThiefPocketTear::Start()
{
	coolDownTime = coolDownMax;

	//준비 모션이 끝나면 베기
	owner->FindClip(readyClip)->SetEvent([this]() { OnReadyEnd(); });
	owner->FindClip(attackClip)->SetEvent([this]() { Finish(); });

	EffectManager::Get()->Play(smokeKey, owner->GlobalPos() + Vector2(0.0f, owner->GetDefaultSize().y * 0.5f), owner->FacingRight());
	owner->PlayClip(readyClip);
}

void ThiefPocketTear::End()
{
	hitbox->SetActive(false);
}

bool ThiefPocketTear::IsReady()
{
	return coolDownTime <= 0.0f;
}

void ThiefPocketTear::SetIcon()
{
	icon = Texture::Add(L"ProjectTextures/Thief/UI_Skill_Icons/PocketTear.png");
}

void ThiefPocketTear::ApplyTier(int newTier)
{
	curData = tierDatas[newTier];

	SetCoolDown(curData.coolDown);
	coolDownTime = min(coolDownTime, coolDownMax);
}

void ThiefPocketTear::OnReadyEnd()
{
	owner->PlayClip(attackClip);
	Slash();
}

void ThiefPocketTear::Slash()
{
	float facing = owner->FacingRight() ? 1.0f : -1.0f;
	Vector2 center = owner->GlobalPos() + Vector2(facing * curData.slashSize.x * 0.25f, owner->GetDefaultSize().y * 0.5f);

	hitbox->Scale() = curData.slashSize;
	hitbox->Pos() = center;
	hitbox->UpdateWorld();
	hitbox->SetActive(true);

	HitData hitData = CreateHitData(0.0f);
	hitData.goldMarkTime = curData.additionalGoldHitTime;
	for (IHittable* hit : owner->GetStage()->CheckAllHittableCollision(hitbox))
	{
		hitData.attackDir = center.x < hit->GetHitPos().x ? 1.0f : -1.0f;
		hit->OnHit(hitData);

		//TODO: 골드 시스템 생기면 curData.goldPerHit 지급 + goldChance 확률로 추가 지급
	}

	EffectManager::Get()->Play(curData.effectKey, center, owner->FacingRight());
}

HitData ThiefPocketTear::CreateHitData(float attackDir)
{
	Stat* stat = owner->GetStatComponent();
	float damage = stat->CalculateFinalAttack(curData.damage, curData.damageType, curData.attackType, AttackActionType::Skill);

	return { damage, stat->GetStat(Attribute::CriticalChance), stat->GetStat(Attribute::CriticalDamage),
	curData.damageType, attackDir, this };
}
