#include "Framework.h"
#include "Enemy.h"
#include "../../Animator.h"
#include "ProjectObjects/Stage.h"
#include "ProjectObjects/Character/Enemy/UI/EnemyHealthBar.h"
#include "EnemyAI.h"


Enemy::Enemy()
{
}

Enemy::~Enemy()
{
	delete hpBar;
	delete ai;
	delete frontSensor;
}

void Enemy::Init()
{
	SetActive(false);
	CreateAnimations(animator);
	PlayClip("Idle");
	InitBody();
	SetStats();
	SetMaxHP();
	SetCurHP(GetMaxHP());
	SetHPBar();

	ai = new EnemyAI(this);
	CreateAttacks(ai);
}

void Enemy::PlayClip(const string& name)
{
	animator->PlayClip(tag + name);
}

Clip* Enemy::FindClip(const string& name)
{
	return animator->FindClip(tag + name);
}

void Enemy::Render()
{
	Character::Render();
	frontSensor->Render();
	ai->Render();	//디버그: 감지 범위/시야선/히트박스
	hpBar->Render();
}

void Enemy::UpdateControl()
{
	if (isKnockBack)
		KnockBack();
	else
		ai->Update();	//판단 결과로 dir이 정해짐 (공격 중이면 0)

	Move();
}

void Enemy::UpdateCollision()
{
	Character::UpdateCollision();

	frontSensor->Pos() = Vector2((defaultSize.x * 0.5f + 8.0f), 0.0f);
	frontSensor->UpdateWorld();
}

void Enemy::PostUpdate()
{
	hpBar->Pos() = GlobalPos() + hpBarOffset;
	hpBar->Update();
}

void Enemy::InitBody()
{
	Character::InitBody();

	frontSensor = new RectCollider({ defaultSize.x * 0.5f, defaultSize.y * 0.2f });
	frontSensor->SetParent(this);
	frontSensor->SetTag("frontSensor");
}

void Enemy::OnHit(const HitData& hit)
{
	Character::OnHit(hit);
}

void Enemy::Die()
{
	Character::Die();
}

void Enemy::StartKnockBack(const HitData& hit)
{
	if (!ai->GetCanMove()) return;

	Character::StartKnockBack(hit);
	ai->CancelAttack();	//맞으면 공격 취소 - 슈퍼아머 적은 여기서 분기

	PlayClip("Hit");

}

void Enemy::EndKnockBack()
{
	Character::EndKnockBack();

	PlayClip("Idle");
}

void Enemy::MoveToward(float dirX)
{
	dir.x = dirX;
	CheckFacing();

	UpdateWorld();
	frontSensor->UpdateWorld();

	if (CannotMoveForward())
		dir.x = 0.0f;
}

void Enemy::FaceTo(Character* target)
{
	float diffX = target->GlobalPos().x - GlobalPos().x;
	if (abs(diffX) < FLT_EPSILON) return;

	dir.x = diffX > 0.0f ? 1.0f : -1.0f;
	CheckFacing();
	ResetMove();	//방향만 바꾸고 이동은 하지 않음
}

void Enemy::Spawn(Vector2 pos)
{
	SetActive(false);

	Vector2 effectPos = pos + Vector2(0.0f, defaultSize.y * 0.5f);
	Effect* effect = EffectManager::Get()->Play("Enemy_Appearance", effectPos);

	effect->OnEnd([this, pos]()
		{
			Appear(pos);
		});
}

bool Enemy::CannotMoveForward()
{
	if (stage->CheckWallTileCollision(frontSensor) != nullptr)
		return true;

	bool onFloor = stage->CheckFloorTileCollision(frontSensor) != nullptr;
	bool onStair = stage->CheckStairTileCollision(frontSensor) != nullptr;
	return !onFloor && !onStair;
}

void Enemy::SetHPBar()
{
	hpBar = new EnemyHealthBar(Vector2(defaultSize.x, 5.0f));
	hpBar->Scale() = SKUL_DEFAULT_ASSET_SIZE;
	hpBar->Init((float)GetMaxHP(), (float)GetCurHP());
	hpBarOffset = Vector2(0.0f, -hpBar->GetBarSize().y);

	SetOnHPChanged([this](float curHP, float maxHP)
		{
			hpBar->SetMaxHP(maxHP);
			hpBar->SetHP(curHP);
		});
}
