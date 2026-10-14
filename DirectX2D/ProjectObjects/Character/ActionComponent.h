#pragma once

class Character;
class ActionState;
class Skill;

class ActionComponent
{
public:
	ActionComponent();
	~ActionComponent();

	void UpdateCoolDown();
	void Update();
	void Render();

	void Start(ActionType type);
	void End();
	bool IsReady(ActionType type);

	bool CanJumpCancel();
	bool CanPreInputJump();

	void CreateActionStates(Character* owner, const map<ActionType, vector<string>>& actionClips, const vector<AttackData>& basicAttackCombos, DamageType basicAttackDamageType);
	void ResetActionStates(const map<ActionType, vector<string>>& actionClips);
	ActionType GetCurActionState() { return curActionState; }

	void SetSkillStateSkill(Skill* skill);

private:
	map<ActionType, ActionState*> actions;
	ActionType curActionState;

};

