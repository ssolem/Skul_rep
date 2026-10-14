#pragma once

class Character;

class Skill
{
public:
	Skill(Character* owner ,float coolDownTime);
	virtual ~Skill();

	virtual void Init() = 0;

	virtual void Update();
	virtual void UpdateCoolDown();
	virtual void Render();

	virtual void Start();
	virtual void End();
	virtual bool IsReady();
	virtual void SetIcon() = 0;

	AttackActionType GetType() { return type; }
	DamageType GetDamageType() { return skillDamageType; }
	void SetIndex(int x) { index = x; }

	Texture* GetIcon() { return icon; }

protected:
	Character* owner;
	AttackActionType type = AttackActionType::Skill;
	DamageType skillDamageType = DamageType::Physical;   // 데미지 없는 스킬(BasicSkullSkill1/2 등)은 안 써도 되지만, 나중에 데미지 스킬이 생기면 여기 재설정
	int baseDamage = 0;	//데미지 있는 스킬만 생성자에서 재설정 (초기화 안 하면 쓰레기값이 들어가 데미지가 0 처리됨)

	int index;
	bool usedSkill;
	bool isReady;
	float coolDownTime;
	float coolDownMax;

	Texture* icon = nullptr;
};

