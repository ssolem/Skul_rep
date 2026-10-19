#include "Framework.h"
#include "ThiefBackAttack.h"
#include "ProjectObjects/Character/Character.h"
#include "ProjectObjects/Character/Enemy/Normal/Enemy.h"
#include "../ThiefSkul.h"
#include "ProjectObjects/Character/Stat.h"
#include "ProjectObjects/Stage.h"

ThiefBackAttack::ThiefBackAttack(Character* owner, ThiefSkul* thiefSkul)
	: Skill(owner, 0), thiefSkul(thiefSkul)
{
	hitbox = new RectCollider(Vector2::One);
	hitbox->SetActive(false);
	//Ready/Attack 두 클립이라 clipName은 비우고 직접 재생 + Finish()
}

ThiefBackAttack::~ThiefBackAttack()
{
	delete hitbox;
}

void ThiefBackAttack::Init()
{
	//struct TierData { damage, coolDown, goldPerHit, invincible, teleport, stunChance, alwaysCritical, effectKey, slashSize }
	TierData data;
	data = { 20.0f, 12.0f,  4, true,  false, 0.0f, false, "ThiefBackAttack",  Vector2(164.0f, 132.0f) };
	tierDatas.push_back(data);
	data = { 20.0f, 12.0f,  6, false, true,  0.0f, false, "ThiefBackAttack",  Vector2(164.0f, 132.0f) };
	tierDatas.push_back(data);
	data = { 25.0f, 12.0f,  8, false, true,  0.5f, false, "Thief4BackAttack", Vector2(394.0f, 212.0f) };
	tierDatas.push_back(data);
	data = { 30.0f, 12.0f, 10, false, true,  0.5f, true,  "Thief4BackAttack", Vector2(394.0f, 212.0f) };
	tierDatas.push_back(data);

	ApplyTier(thiefSkul->GetTier());
	coolDownTime = 0.0f;
}

void ThiefBackAttack::Update()
{
}

void ThiefBackAttack::UpdateCoolDown()
{
	if (coolDownTime > 0.0f)
		coolDownTime -= DELTA;
}

void ThiefBackAttack::Render()
{
	if (hitbox->Active())
		hitbox->Render();
}

void ThiefBackAttack::Start()
{
	coolDownTime = coolDownMax;

	if (curData.invincible)
		owner->SetInvincible(true);

	//준비 모션이 끝나면 순간이동 + 베기
	owner->FindClip(readyClip)->SetEvent([this]() { OnReadyEnd(); });
	owner->FindClip(attackClip)->SetEvent([this]() { Finish(); });

	owner->PlayClip(readyClip);
}

void ThiefBackAttack::End()
{
	//정상 종료/취소 모두 무적 해제
	owner->SetInvincible(false);
	hitbox->SetActive(false);
}

bool ThiefBackAttack::IsReady()
{
	return coolDownTime <= 0.0f;
}

void ThiefBackAttack::SetIcon()
{
	icon = Texture::Add(L"ProjectTextures/Thief/UI_Skill_Icons/Backattack.png");
}

void ThiefBackAttack::ApplyTier(int newTier)
{
	curData = tierDatas[newTier];

	SetCoolDown(curData.coolDown);
	coolDownTime = min(coolDownTime, coolDownMax);
}

void ThiefBackAttack::OnReadyEnd()
{
	if (curData.teleport)
	{
		Enemy* target = owner->GetStage()->FindClosestEnemy(owner->GlobalPos(), teleportRange);
		if (target)
			TeleportBehind(target);
	}

	owner->PlayClip(attackClip);
	Slash();
}

void ThiefBackAttack::TeleportBehind(Enemy* enemy)
{
	//적이 바라보는 반대쪽(등 뒤)으로 이동해서 적을 바라봄
	float backDir = enemy->FacingRight() ? -1.0f : 1.0f;

	owner->Pos() = Vector2(enemy->GlobalPos().x + backDir * behindOffset, enemy->GlobalPos().y);
	owner->SetVelocity(Vector2::Zero);
	owner->SetFacing(backDir < 0.0f);
	owner->UpdateWorld();
}

void ThiefBackAttack::Slash()
{
	float facing = owner->FacingRight() ? 1.0f : -1.0f;
	Vector2 center = owner->GlobalPos() + Vector2(facing * curData.slashSize.x * 0.25f, owner->GetDefaultSize().y * 0.5f);

	hitbox->Scale() = curData.slashSize;
	hitbox->Pos() = center;
	hitbox->UpdateWorld();
	hitbox->SetActive(true);

	HitData hitData = CreateHitData(0.0f);
	for (IHittable* hit : owner->GetStage()->CheckAllHittableCollision(hitbox))
	{
		hitData.attackDir = center.x < hit->GetHitPos().x ? 1.0f : -1.0f;
		hitData.stunTime = Random(0.0f, 1.0f) < curData.stunChance ? stunTime : 0.0f;
		hit->OnHit(hitData);

		//TODO: 골드 시스템 생기면 curData.goldPerHit 지급
	}

	EffectManager::Get()->Play(curData.effectKey, center, owner->FacingRight());
}

HitData ThiefBackAttack::CreateHitData(float attackDir)
{
	Stat* stat = owner->GetStatComponent();
	float damage = stat->CalculateFinalAttack(curData.damage, curData.damageType, curData.attackType, AttackActionType::Skill);
	float criticalChance = curData.alwaysCritical ? 1.0f : stat->GetStat(Attribute::CriticalChance);

	return { damage, criticalChance, stat->GetStat(Attribute::CriticalDamage),
	curData.damageType, attackDir, this };
}

