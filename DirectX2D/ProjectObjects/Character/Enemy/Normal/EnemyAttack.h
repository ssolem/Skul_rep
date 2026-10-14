#pragma once

class Enemy;
class Character;

//공격 패턴
class EnemyAttack
{
public:
	EnemyAttack(Enemy* owner, float coolDown);
	virtual ~EnemyAttack();

	void UpdateCoolDown();

	virtual bool CanAttack(Character* target);	
	virtual void Start(Character* target);
	virtual void Update() {}
	virtual void Cancel() = 0;
	virtual void Render();

	bool IsFinished() { return finished; }
	int GetPriority() { return priority; }
	void SetPriority(int p) { priority = p; }
	float GetWeight() { return weight; }
	void SetWeight(float w) { weight = w; }

protected:
	Enemy* owner;
	Character* target = nullptr;

	float coolDown;
	float coolDownTime = 0.0f;
	int priority = 0;		//공격 우선순위
	float weight = 1.0f;	//같은 우선순위 안에서 뽑힐 비율
	bool finished = true;

	Collider* rangeCollider = nullptr;
};
