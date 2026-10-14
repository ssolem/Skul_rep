#include "Framework.h"
#include "GiganticEnt.h"
#include "../../Animator.h"
#include "../../Stat.h"
#include "EnemyAI.h"
#include "GiganticEntMelee.h"
#include "GiganticEntRange.h"

GiganticEnt::GiganticEnt()
{
	SetTag("GiganticEnt");
	Init();
}

GiganticEnt::~GiganticEnt()
{
}

void GiganticEnt::CreateAnimations(Animator* animator)
{
	animator->CreateClipByAtlas(tag, "Attack_Melee", L"ProjectTextures/Chapter1_GiganticEnt/Atlas", false);
	animator->FindClip(tag + "Attack_Melee")->SetFrameDelay(3, 0.7f);
	animator->CreateClipByAtlas(tag, "Attack_Range", L"ProjectTextures/Chapter1_GiganticEnt/Atlas", false);
	animator->FindClip(tag + "Attack_Range")->SetFrameDelay(1, 0.7f);
	animator->CreateClipByAtlas(tag, "Idle", L"ProjectTextures/Chapter1_GiganticEnt/Atlas");
}

void GiganticEnt::SetStats()
{
	stat->AddStat({ StatType::BaseStat, Attribute::Health, 150.0f });
}

void GiganticEnt::InitBody()
{
	Enemy::InitBody();

	collider->Pos() += { 0.0f, defaultSize.y * 0.5f };
	collider->Scale().x = 0.75f;
	collider->UpdateWorld();

	groundSensor->Scale() = { 0.75f, 0.05f };
	groundSensor->Pos() += { 0.0f, 0.0f };
	groundSensor->UpdateWorld();
}

void GiganticEnt::CreateAttacks(EnemyAI* ai)
{
	ai->SetCanMove(false);

	ai->AddAttack(new GiganticEntMelee(this, 4, 5.0f, 200.f));

	EnemyAttack* range = new GiganticEntRange(this, 2, 5.0f, 400.f);
	range->SetPriority(1);
	ai->AddAttack(range);

}

