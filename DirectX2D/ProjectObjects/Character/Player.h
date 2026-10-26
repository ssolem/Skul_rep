#pragma once
#include "Character.h"

class ObjectController;
class ActionComponent;
class PlayerInfoUI;
class SkulClass;

class Player : public Character
{
public:
	struct PreInput
	{
		ActionType type;
		float remainTime = 0.0f;
		float dirX = 0.0f;
		bool canUse = false;
	};

public:
	Player();
	~Player() override;

	virtual void PlayClip(const string& name) override;
	virtual Clip* FindClip(const string& name) override;
	virtual void Render() override;

	SkulClass* GetCurSkul() { return skuls[curSkulIndex]; }
	SkulClass* GetSubSkul()
	{ 
		if (skuls.size() < 2) return nullptr;
		if (skuls[1] == nullptr) return nullptr;
		else return skuls[(curSkulIndex + 1) % 2];
	}
	//virtual Skill* GetCurSkill() override;

	//재화 - 드랍 아이템 획득 시 증가, HUD가 읽어감
	void AddGold(int amount) { gold += amount; }
	void AddDarkQuartz(int amount) { darkQuartz += amount; }
	void AddBoneChip(int amount) { boneChip += amount; }
	int GetGold() { return gold; }
	int GetDarkQuartz() { return darkQuartz; }
	int GetBoneChip() { return boneChip; }

	void SetRefreshPlayerInfo(Event event) { refreshPlayerInfo = event; }

protected:
	void UpdateControl() override;
	void PostUpdate() override;
	void InitBody() override;

	//interface
	virtual void OnHit(const HitData& hit) override;
	virtual void TakeDamage(int amount) override;
	virtual void Die() override;

	virtual void StartKnockBack(const HitData& hit) override;
	virtual void EndKnockBack() override;


private:
	void TryAction(ActionType type);
	void DoAction(ActionType type);
	void CancelAction();
	bool CanAction(ActionType type);
	void SwitchSkul();

	void UpdatePreInput();

	void DoJump();
	void OnJumpPressed();
	void DoSkill1();
	void DoSkill2();

	float GetInputDirx();

	void SetupInput();
	void SetStats();
	void CreateEffects();

	void SetEffectDetails();

private:
	ObjectController* controller = nullptr;
	ActionComponent* actionComponent = nullptr;

	vector<SkulClass*> skuls;
	int curSkulIndex = 0;

	PreInput preInput;
	float preInputTime = 0.4f;
	Vector2 jumpEffectOffset = Vector2::Zero;

	int gold = 0;
	int darkQuartz = 0;
	int boneChip = 0;

	Event refreshPlayerInfo;
};
