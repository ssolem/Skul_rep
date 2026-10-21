#include "Framework.h"
#include "Ent.h"
#include "../../Animator.h"
#include "../../Stat.h"
#include "EnemyAI.h"
#include "MeleeSwing.h"

Ent::Ent()
{
	SetTag("Ent");
	Init();
}

Ent::~Ent()
{
}

void Ent::CreateAnimations(Animator* animator)
{
	animator->CreateClipByAtlas(tag, "Attack", L"ProjectTextures/Chapter1_Ent/Atlas", false);
	animator->FindClip(tag + "Attack")->SetFrameDelay(0, 0.7f);	//0번(기 모으기)만 길게 - 나머지 휘두르기는 기본 0.1초
	animator->CreateClipByAtlas(tag, "Hit", L"ProjectTextures/Chapter1_Ent/Atlas", false);
	animator->CreateClipByAtlas(tag, "Idle", L"ProjectTextures/Chapter1_Ent/Atlas");
	animator->CreateClipByAtlas(tag, "Walk", L"ProjectTextures/Chapter1_Ent/Atlas");
}

void Ent::SetStats()
{
	stat->AddStat({ StatType::BaseStat, Attribute::Health, 30.0f });
	stat->AddStat({ StatType::BaseStat, Attribute::MovementSpeed, 200.0f });

	//드랍 보상
	dropTable.goldCount = 2;
	dropTable.goldAmount = 2;
}

void Ent::InitBody()
{
	Enemy::InitBody();

	collider->Pos() += { 0.0f, defaultSize.y * 0.5f };
	collider->Scale().x = 0.75f;
	collider->UpdateWorld();

	groundSensor->Scale() = { 0.75f, 0.05f };
	groundSensor->Pos() += { 0.0f, 0.0f };
	groundSensor->UpdateWorld();
}

void Ent::CreateAttacks(EnemyAI* ai)
{
	AttackData swing;
	swing.clipName = "Attack";
	swing.damage = 8;
	swing.type = AttackType::Melee;
	swing.hitboxSize = FindClip("Attack")->GetFrame(1)->GetSize();
	swing.hitboxOffset = { 0.0f, 0.0f };

	ai->AddAttack(new MeleeSwing(this, swing, 1, 2.0f));
}
