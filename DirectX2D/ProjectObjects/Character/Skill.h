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

	//단계가 있는 스컬(Thief 등)만 재정의 - 기본은 아무것도 안 함

	virtual void ApplyTier(int newTier) {}

	AttackActionType GetType() { return type; }
	DamageType GetDamageType() { return skillDamageType; }
	void SetIndex(int x) { index = x; }
	void SetCoolDown(float coolDown) { coolDownMax = coolDown; }
	virtual float GetCoolDownRatio()
	{
		if (coolDownMax <= 0.0f) return 1.0f;
		return 1.0f - max(0.0f, min(coolDownTime / coolDownMax, 1.0f));
	}

	//비어 있으면 스킬이 클립 재생/종료를 직접 관리
	const string& GetClipName() { return clipName; }

	//SkillState가 넣어 주는 종료 콜백
	void SetOnFinish(function<void()> callback) { onFinish = callback; }

	Texture* GetIcon() { return icon; }

protected:
	//clipName이 빈 스킬은 끝나는 시점에 직접 호출
	void Finish() { if (onFinish) onFinish(); }

	//사용 시 호출 - 기본 UpdateCoolDown을 쓰는 스킬용
	void StartCoolDown() { isReady = false; usedSkill = true; coolDownTime = coolDownMax; }

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

	string clipName;	//클립 하나짜리 스킬만 생성자에서 설정 (태그 없는 이름, 스컬 태그는 Player가 붙임)
	function<void()> onFinish;

	Texture* icon = nullptr;
};

