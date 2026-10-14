#include "Framework.h"
#include "RootEnt.h"
#include "../../Animator.h"
#include "../../Stat.h"
#include "EnemyAI.h"
#include "RootEntAttack.h"

RootEnt::RootEnt()
{
	SetTag("RootEnt");
	Init();
}

RootEnt::~RootEnt()
{
}

void RootEnt::CreateAnimations(Animator* animator)
{
	animator->CreateClipByAtlas(tag, "Attack", L"ProjectTextures/Chapter1_RootEnt/Atlas", false);
	animator->FindClip(tag + "Attack")->SetFrameDelay(2, 0.7f);
	animator->CreateClipByAtlas(tag, "Hit", L"ProjectTextures/Chapter1_RootEnt/Atlas", false);
	animator->CreateClipByAtlas(tag, "Idle", L"ProjectTextures/Chapter1_RootEnt/Atlas");
	animator->CreateClipByAtlas(tag, "Walk", L"ProjectTextures/Chapter1_RootEnt/Atlas");
}

void RootEnt::SetStats()
{
	stat->AddStat({ StatType::BaseStat, Attribute::Health, 40.0f });
	stat->AddStat({ StatType::BaseStat, Attribute::MovementSpeed, 180.0f });
}

void RootEnt::InitBody()
{
	Enemy::InitBody();

	collider->Pos() += { 0.0f, defaultSize.y * 0.5f };
	collider->Scale().x = 0.75f;
	collider->UpdateWorld();

	groundSensor->Scale() = { 0.75f, 0.05f };
	groundSensor->Pos() += { 0.0f, 0.0f };
	groundSensor->UpdateWorld();
}

void RootEnt::CreateAttacks(EnemyAI* ai)
{
	ai->AddAttack(new RootEntAttack(this, 3, 3, 200.0f));
}
