#include "Framework.h"
#include "BasicSkullSkill1.h"
#include "ProjectObjects/Character/Character.h"
#include "../BasicSkul.h"
#include "ProjectObjects/Stage.h"
#include "ProjectObjects/Objects/SkulObject.h"
#include "../../Stat.h"
#include "../../Enemy/Normal/Enemy.h"

BasicSkullSkill1::BasicSkullSkill1(Character* owner, float coolDown, BasicSkul* basicSkul)
	: Skill(owner, coolDown), basicSkul(basicSkul)
{
	baseDamage = 10;
	clipName = "Skill";	//머리 던지기 클립 끝에서 SkillState가 End
}

BasicSkullSkill1::~BasicSkullSkill1()
{
}

void BasicSkullSkill1::Init()
{
	//머리(SkulObject) 생성/충돌 설정은 BasicSkul::CreateHead()가 담당 (2번 스킬도 같은 머리를 참조해야 해서 이관함)
	head = basicSkul->GetHead();
}

void BasicSkullSkill1::OnHeadHit(IHittable* target)
{
	Stat* stat = owner->GetStatComponent();

	HitData data;
	data.attackDir = head->GetVelocity().x > 0.0f ? 1.0f : -1.0f;
	data.damage = stat->CalculateFinalAttack((float)baseDamage, skillDamageType, AttackType::Projectile, AttackActionType::Skill);
	data.criticalChance = stat->GetStat(Attribute::CriticalChance);
	data.criticalDamage = stat->GetStat(Attribute::CriticalDamage);
	data.damageType = skillDamageType;
	data.source = this;

	target->OnHit(data);

	//flightState 바뀌면 BasicSkul이 더 이상 적 충돌을 검사하지 않음 -> 한 번 던지면 한 번만 타격
	head->ActivatePhysics();
	head->Bounce(Vector2(-data.attackDir, 0.0f), 0.6f, 160.0f);
}

void BasicSkullSkill1::OnHeadHitObject(SkulObject* object)
{
	if (find(alreadyHit.begin(), alreadyHit.end(), object) != alreadyHit.end())
		return;	//이번 던지기에서 이미 날린 오브젝트

	alreadyHit.push_back(object);
	head->KnockAway(object);	//적과 달리 머리는 그대로 날아가고 상대만 날림
}

void BasicSkullSkill1::Start()
{
	isReady = false;
	usedSkill = true;
	alreadyHit.clear();

	basicSkul->RegisterHeadToStage(owner->GetStage());

	basicSkul->SetTag("BasicSkulHeadless");
	rightDirection = owner->FacingRight();

	float dirX = rightDirection ? 1.0f : -1.0f;

	throwPosOffset = { dirX * 20.0f, owner->GetDefaultSize().y * 0.85f };
	head->Pos() = owner->GlobalPos() + throwPosOffset;

	head->Scale().x = SKUL_DEFAULT_ASSET_SIZE.x * dirX;

	head->UpdateWorld();
	head->GetCollider()->UpdateWorld();	//던지는 프레임부터 콜라이더도 실제 위치에 있어야 회수 판정(canRetrieveHead)이 틀어지지 않음
	head->SetActive(true);

	Vector2 startPos = head->Pos();
	Vector2 launchVelocity = { dirX * headSpeed, 0.0f };
	head->SetVelocity(launchVelocity);

	head->StartLaunch([startPos, launchVelocity, this](SkulObject* self, float dt)
		{
			self->Pos() += launchVelocity * dt;

			if ((self->Pos() - startPos).Length() >= throwRange)
				self->ActivatePhysics();
		});
}

void BasicSkullSkill1::End()
{
}

bool BasicSkullSkill1::IsReady()
{
	return isReady && !(head->Active());
}

void BasicSkullSkill1::SetIcon()
{
	icon = Texture::Add(L"ProjectTextures/Skul_Images/SkullThrowing.png");
}
