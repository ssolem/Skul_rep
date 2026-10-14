#include "Framework.h"
#include "BlossomEnt.h"
#include "../../Animator.h"
#include "../../Stat.h"
#include "EnemyAI.h"
#include "BlossomEntAttack.h"

BlossomEnt::BlossomEnt()
{
	SetTag("BlossomEnt");
	Init();
}

BlossomEnt::~BlossomEnt()
{
}

void BlossomEnt::CreateAnimations(Animator* animator)
{
	animator->CreateClipByAtlas(tag, "Attack_Ready", L"ProjectTextures/Chapter1_BlossomEnt/Atlas", false);
	animator->CreateClipByAtlas(tag, "Attack", L"ProjectTextures/Chapter1_BlossomEnt/Atlas");
	animator->CreateClipByAtlas(tag, "Hit", L"ProjectTextures/Chapter1_BlossomEnt/Atlas", false);
	animator->CreateClipByAtlas(tag, "Idle", L"ProjectTextures/Chapter1_BlossomEnt/Atlas");
	animator->CreateClipByAtlas(tag, "Walk", L"ProjectTextures/Chapter1_BlossomEnt/Atlas");
}

void BlossomEnt::SetStats()
{
	stat->AddStat({ StatType::BaseStat, Attribute::Health, 40.0f });
	stat->AddStat({ StatType::BaseStat, Attribute::MovementSpeed, 180.0f });
}

void BlossomEnt::InitBody()
{
	Enemy::InitBody();

	collider->Pos() += { 0.0f, defaultSize.y * 0.5f };
	collider->Scale().x = 0.75f;
	collider->UpdateWorld();

	groundSensor->Scale() = { 0.75f, 0.05f };
	groundSensor->Pos() += { 0.0f, 0.0f };
	groundSensor->UpdateWorld();
}

void BlossomEnt::CreateAttacks(EnemyAI* ai)
{
	ai->AddAttack(new BlossomEntAttack(this, 5.0f, 100.0f));
}
