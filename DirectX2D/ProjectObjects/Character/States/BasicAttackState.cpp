#include "Framework.h"
#include "BasicAttackState.h"
#include "../../Stage.h"
#include "../Stat.h"

BasicAttackState::BasicAttackState(Character* character, ActionType type, const vector<AttackData>& combos, DamageType damageType)
	: ActionState(character, type, ExtractClipNames(combos)),
	combos(combos), damageType(damageType), maxComboIndex(combos.size()), curComboIndex(0)
{
	finishedAttack = false;
	isAttacking = false;
	attackCollider = character->GetAttackCollider();
	attackCollider->Pos() = { 0, 0 };
}

BasicAttackState::~BasicAttackState()
{
}

void BasicAttackState::Update()
{
	CheckHit();
	AttackMovement();
}

void BasicAttackState::UpdateCoolDown()
{
	if (isAttacking) return;

	if (curComboIndex != 0)
	{
		comboTimer -= DELTA;
		if (comboTimer <= 0.0f)
			curComboIndex = 0;
	}
}

void BasicAttackState::Render()
{
	if (attackCollider != nullptr)
		attackCollider->Render();
}

void BasicAttackState::Start()
{
	finishedAttack = false;
	isAttacking = true;

	curAttackIndex = curComboIndex % maxComboIndex;
	const AttackData& data = combos[curAttackIndex];
	curClipName = data.clipName;

	// 이번 콤보의 이동 거리 / 지속 시간 → 목표 속도
	distance = data.moveDistance;
	float duration = data.moveDuration;
	moveSpeed = (duration > 0.0f) ? (distance / duration) : 0.0f;
	moveTimer = duration;
	traveled = 0.0f;
	moveDirection = Vector2::Zero;	// 방향은 AttackMovement에서 매 프레임 입력으로 결정

	alreadyHit.clear();

	((RectCollider*)attackCollider)->SetSize(data.hitboxSize);
	attackCollider->Pos().y += data.hitboxSize.y * 0.5f;
	attackCollider->Pos() += data.hitboxOffset;
	attackCollider->SetActive(true);

	curComboIndex = (curComboIndex + 1) % maxComboIndex;

	Clip* clip = owner->FindClip(curClipName);
	clip->SetEvent([this]() { owner->SetCancellable(true); }, 0);
	clip->SetEvent([this]() { End(); });

	PlayClip();
}

void BasicAttackState::End()
{
	attackCollider->SetActive(false);
	attackCollider->Pos() = { 0, 0 };

	owner->OnEndAction();
	isAttacking = false;
	moveTimer = 0.0f;

	if (curComboIndex != 0)
	{
		comboTimer = comboInputTime;
		finishedAttack = false;
	}
	else
	{
		finishedAttack = true;
	}
}

bool BasicAttackState::IsReady()
{
	return owner->IsGrounded() && !isAttacking;
}

void BasicAttackState::AttackMovement()
{
	if (moveTimer <= 0.0f) return;
	moveTimer -= DELTA;

	Vector2 inputDir = owner->GetDirection();
	if (inputDir.Length() < FLT_EPSILON) return;

	moveDirection.x = (inputDir.x > 0.0f) ? 1.0f : -1.0f;
	moveDirection.y = 0.0f;
	owner->CheckFacing();

	float step = moveSpeed * DELTA;
	if (traveled + step > distance)
		step = distance - traveled;

	owner->Pos().x += moveDirection.x * step;
	traveled += step;
	attackCollider->UpdateWorld();
}

//ActionState 생성자가 필요로 하는 클립 이름 목록을 combos에서 뽑아낸다
vector<string> BasicAttackState::ExtractClipNames(const vector<AttackData>& combos)
{
	vector<string> names;
	for (const AttackData& combo : combos)
		names.push_back(combo.clipName);
	return names;
}

void BasicAttackState::CheckHit()
{
	attackCollider->UpdateWorld();

	owner->CheckFacing();

	const AttackData& data = combos[curAttackIndex];
	float finalDamage = owner->GetStatComponent()->CalculateFinalAttack(data.damage, damageType, data.type, AttackActionType::Basic);
	float critChance = owner->GetStatComponent()->GetStat(Attribute::CriticalChance);
	float critDamage = owner->GetStatComponent()->GetStat(Attribute::CriticalDamage);

	HitData hitData = { finalDamage, critChance, critDamage, damageType, owner->FacingRight() ? 1.0f : -1.0f, this };

	for (IHittable* hit : owner->GetStage()->CheckAllHittableCollision(attackCollider))
	{
		if (find(alreadyHit.begin(), alreadyHit.end(), hit) != alreadyHit.end())
			continue;   // 이번 공격에서 이미 맞춘 적 - 스킵

		alreadyHit.push_back(hit);
		hit->OnHit(hitData);
	}
}
