#include "Framework.h"
#include "EnemyAttack.h"
#include "ProjectObjects/Character/Character.h"

EnemyAttack::EnemyAttack(Enemy* owner, float coolDown)
	: owner(owner), coolDown(coolDown)
{
}

EnemyAttack::~EnemyAttack()
{
	if (rangeCollider)
		delete rangeCollider;
}

void EnemyAttack::UpdateCoolDown()
{
	if (coolDownTime > 0.0f)
		coolDownTime -= DELTA;
}

//Äð´Ù¿î + rangeCollider(ÀÖÀ¸¸é)¿¡ Å¸°ÙÀÌ ´ê´ÂÁö
bool EnemyAttack::CanAttack(Character* target)
{
	if (!finished || coolDownTime > 0.0f) return false;
	if (rangeCollider == nullptr) return true;

	rangeCollider->UpdateWorld();
	return rangeCollider->IsCollision(target->GetCollider());
}

void EnemyAttack::Render()
{
	if (rangeCollider)
		rangeCollider->Render();
}

void EnemyAttack::Start(Character* target)
{
	this->target = target;
	finished = false;
	coolDownTime = coolDown;
}
