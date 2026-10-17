#include "Framework.h"
#include "ThiefBackRoll.h"
#include "ProjectObjects/Character/Character.h"
#include "../ThiefSkul.h"
#include "ProjectObjects/Character/Stat.h"
#include "ProjectObjects/Stage.h"

//Skill2
ThiefBackRoll::ThiefBackRoll(Character* owner, ThiefSkul* thiefSkul)
	: Skill(owner, 0), thiefSkul(thiefSkul)
{
	hitbox = new RectCollider(Vector2::One);
	hitbox->SetActive(false);
	clipName = "Skill2";
}

ThiefBackRoll::~ThiefBackRoll()
{
	delete hitbox;
}

void ThiefBackRoll::Init()
{
	//struct TierData
	//{
	//	float damage;
	//	float coolDown;
	//	int maxCharges;
	//	BackRollType smokeType;
	//	DamageType damageType;
	// AttackType attackType;
	//};

	TierData data;
	data = { 8.0f, 10.0f, 3, BackRollType::Smoke, DamageType::Magic };
	tierDatas.push_back(data);
	data = { 8.0f, 10.0f, 3, BackRollType::Caltrap, DamageType::Physical, AttackType::Projectile };
	tierDatas.push_back(data);
	data = { 10.0f, 10.0f, 3, BackRollType::Plague, DamageType::Magic };
	tierDatas.push_back(data);
	data = { 15.0f, 20.0f, 1, BackRollType::Plague, DamageType::Magic };
	tierDatas.push_back(data);

	CreateCaltrap();

	curData = tierDatas[0];

	SetCoolDown(curData.coolDown);
	coolDownTime = coolDownMax;
	curCharges = curData.maxCharges;
}

void ThiefBackRoll::Update()
{
	Move();
	CheckHit();
}

void ThiefBackRoll::UpdateCoolDown()
{
	if (skillUseCooldown > 0.0f)
		skillUseCooldown -= DELTA;

	if (curCharges >= curData.maxCharges) return;

	coolDownTime -= DELTA;
	if (coolDownTime > 0.0f) return;

	curCharges++;
	coolDownTime = coolDownMax;
}

void ThiefBackRoll::Render()
{
	if (hitbox->Active())
		hitbox->Render();
}

void ThiefBackRoll::Start()
{
	curCharges--;
	skillUseCooldown = skillUseMaxCooldown;
	alreadyHit.clear();
	
	backDir = owner->FacingRight() ? -1.0f : 1.0f;
	movedDistance = 0.0f;

	Clip* clip = owner->FindClip(clipName);
	if (clip != nullptr)
	{
		clip->SetSpeed(1.0f);
		float clipDuration = clip->GetDuration();
		float rollDuration = distance / speed;
		clip->SetSpeed(clipDuration / rollDuration);
	}

	switch (curData.smokeType)
	{
	case BackRollType::Smoke:       OnSmokeAction();        break;
	case BackRollType::Caltrap:     OnCaltrapAction();        break;
	case BackRollType::Plague:      OnPlagueAction();       break;
	}
}

void ThiefBackRoll::End()
{
	hitbox->SetActive(false);
}

bool ThiefBackRoll::IsReady()
{
	return curCharges > 0 && skillUseCooldown <= 0.0f;
}

void ThiefBackRoll::SetIcon()
{
	icon = Texture::Add(L"ProjectTextures/Thief/UI_Skill_Icons/BackRoll.png");
}

void ThiefBackRoll::ApplyTier(int newTier)
{
	curData = tierDatas[newTier];

	SetCoolDown(curData.coolDown);
	curCharges = min(curCharges, curData.maxCharges);
	coolDownTime = min(coolDownTime, coolDownMax);
}

void ThiefBackRoll::Move()
{
	float moveAmount = speed * DELTA;

	if (movedDistance + moveAmount >= distance)
	{
		moveAmount = distance - movedDistance;
	}

	owner->Pos().x += backDir * moveAmount;
	movedDistance += moveAmount;
}

void ThiefBackRoll::CheckHit()
{
	if (hitbox->Active())
	{
		HitData hitData;
		bool hasHitData = false;

		for (IHittable* hit : owner->GetStage()->CheckAllHittableCollision(hitbox))
		{
			if (find(alreadyHit.begin(), alreadyHit.end(), hit) != alreadyHit.end())
				continue;

			if (!hasHitData)
			{
				hitData = CreateHitData(0.0f);
				hasHitData = true;
			}

			hitData.attackDir = hitbox->GlobalPos().x < hit->GetHitPos().x ? 1.0f : -1.0f;
			alreadyHit.push_back(hit);
			hit->OnHit(hitData);
		}
	}
}

void ThiefBackRoll::PlaceHitbox(Vector2 size)
{
	hitbox->Scale() = size;
	hitbox->Pos() = owner->GlobalPos() + Vector2(0.0f, size.y * 0.5f);
	hitbox->UpdateWorld();
	hitbox->SetActive(true);
}

void ThiefBackRoll::OnSmokeAction()
{
	PlaceHitbox(Vector2(30.0f, 40.0f));
	EffectManager::Get()->Play("ThiefSmoke", owner->GlobalPos());
}

void ThiefBackRoll::OnCaltrapAction()
{
	HitData hitData = CreateHitData(0.0f);
	Vector2 pos = owner->GlobalPos() + Vector2(0.0f, owner->GetDefaultSize().y * 0.5f);

	for (int i = 0; i < caltrapCount; i++)
	{
		float angle = XMConvertToRadians(30.0f + 20.0f * i);
		Vector2 dir = { cosf(angle) * -backDir, sinf(angle) };
		ProjectileManager::Get()->Fire(caltrapTag, pos, dir, hitData, ProjectileTarget::Enemy);
	}
}

void ThiefBackRoll::OnPlagueAction()
{
	PlaceHitbox(Vector2(150.0f, 100.0f));
	EffectManager::Get()->Play("ThiefPlague", hitbox->GlobalPos());
}

void ThiefBackRoll::CreateCaltrap()
{
	ProjectileData data;
	data.tag = caltrapTag;
	if (ProjectileManager::Get()->HasData(data.tag)) return;

	data.path = L"ProjectTextures/Thief/Projectile";
	data.flyClip = "Thief_Caltrap";
	data.hitClip = "";
	data.lifeTime = 5.0f;
	data.size = Vector2::Zero;
	data.speed = caltrapSpeed;
	data.tileHitType = TileHitType::Stick;
	data.pierce = false;
	data.gravity = GRAVITY;
	data.rotateToVelocity = false;

	ProjectileManager::Get()->Register(data.tag, data);
}

HitData ThiefBackRoll::CreateHitData(float attackDir)
{
	Stat* stat = owner->GetStatComponent();
	float damage = stat->CalculateFinalAttack(curData.damage, curData.damageType, curData.attackType, AttackActionType::Skill);

	return { damage, stat->GetStat(Attribute::CriticalChance), stat->GetStat(Attribute::CriticalDamage),
	curData.damageType, attackDir, this };
}

