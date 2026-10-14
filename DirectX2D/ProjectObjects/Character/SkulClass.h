#pragma once

class Skill;
class Animator;
class Character;
class Stat;

class SkulClass
{
public:
	SkulClass();
	virtual ~SkulClass();

	virtual void Update();
	virtual void Render();

	virtual void Init(Character* owner, Animator* animator) = 0;	//owner는 여기서 멤버에 저장하고, 이후 함수들은 멤버를 사용
	virtual void CreateAnimations(Animator* animator) = 0;
	virtual void SetAnimations() = 0;
	virtual void CreateEffects();
	virtual void SetEffectDetails();
	virtual void SetStats() = 0;
	virtual void SetImages() = 0;

	const string& GetTag() { return tag; }
	const map<ActionType, vector<string>>& GetActionClipNames() const { return actionClips; }
	vector<Skill*>& GetCurSkill() { return curSkills; }

	Texture* GetMainPortrait() { return mainPortrait; }
	Texture* GetSubPortrait() { return subPortrait; }
	Texture* GetInfoPortrait() { return infoPortrait; }

	//기본 공격 콤보 데이터가 없는 SkulClass는 그냥 빈 벡터 (BasicSkul처럼 콤보가 있는 쪽만 오버라이드)
	virtual const vector<AttackData>& GetComboAttacks() const
	{
		static vector<AttackData> empty;
		return empty;
	}

	DamageType GetBasicAttackDamageType() const { return basicAttackDamageType; }

	void SetTag(const string& tagName) { tag = tagName; }

protected:
	virtual void CreateSkills() = 0;
	virtual void CreateComboAttacks(Animator* animator) = 0;

protected:
	string tag = "";
	Character* owner = nullptr;
	map<ActionType, vector<string>> actionClips;
	vector<Skill*> curSkills;
	vector<Skill*> skills;

	Stat* stat;
	vector<AttackData> basicAttackCombos;
	AttackData jumpAttack;

	Texture* mainPortrait = nullptr;
	Texture* subPortrait = nullptr;
	Texture* infoPortrait = nullptr;

	DamageType basicAttackDamageType = DamageType::Physical;   // 기본값 - Magic/Fixed 무기인 Skul은 SetStats()에서 재설정

	int curSkillIndex = 0;

};

