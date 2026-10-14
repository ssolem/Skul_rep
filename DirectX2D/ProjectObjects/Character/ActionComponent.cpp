#include "Framework.h"
#include "ActionComponent.h"
#include "States/DashState.h"
#include "States/BasicAttackState.h"
#include "States/JumpAttackState.h"
#include "States/SkillState.h"
#include "States/SwitchState.h"


ActionComponent::ActionComponent()
{
}

ActionComponent::~ActionComponent()
{
	for (auto& pair : actions)
		delete pair.second;
}

void ActionComponent::UpdateCoolDown()
{
	for (auto& pair : actions)
		pair.second->UpdateCoolDown();
}

void ActionComponent::Update()
{
	actions[curActionState]->Update();
}

void ActionComponent::Render()
{
	actions[curActionState]->Render();
}

void ActionComponent::Start(ActionType type)
{
	curActionState = type;
	actions[curActionState]->Start();
	//actions[curActionState]->PlayClip();
}

void ActionComponent::End()
{
	actions[curActionState]->End();
}

bool ActionComponent::IsReady(ActionType type)
{
	auto it = actions.find(type);
	if (it != actions.end())
		return it->second->IsReady();

	switch (type)
	{
	case ActionType::SKILL:       return true;
	case ActionType::SWITCH:      return true;
	}
	return false;
}

bool ActionComponent::CanJumpCancel()
{
	switch (curActionState)
	{
	case ActionType::BASIC_ATTACK:		return true;
	case ActionType::JUMP_ATTACK:		return false;
	case ActionType::DASH:				return true;
	case ActionType::SKILL:				return false;
	case ActionType::SWITCH:			return false;
	default:							return false;
	}
}

bool ActionComponent::CanPreInputJump()
{
	switch (curActionState)
	{
	case ActionType::SKILL:  return true;
	default:                 return false;
	}
}

void ActionComponent::CreateActionStates(Character* owner, const map<ActionType, vector<string>>& actionClips, const vector<AttackData>& basicAttackCombos, DamageType basicAttackDamageType)
{
	ActionState* newAction;
	newAction = new DashState(owner, ActionType::DASH, actionClips.at(ActionType::DASH));
	actions[ActionType::DASH] = newAction;

	newAction = new BasicAttackState(owner, ActionType::BASIC_ATTACK, basicAttackCombos, basicAttackDamageType);
	actions[ActionType::BASIC_ATTACK] = newAction;

	newAction = new JumpAttackState(owner, ActionType::JUMP_ATTACK, actionClips.at(ActionType::JUMP_ATTACK));
	actions[ActionType::JUMP_ATTACK] = newAction;

	newAction = new SkillState(owner, ActionType::SKILL, actionClips.at(ActionType::SKILL));
	actions[ActionType::SKILL] = newAction;

	newAction = new SwitchState(owner, ActionType::SWITCH, actionClips.at(ActionType::SWITCH));
	actions[ActionType::SWITCH] = newAction;
}

void ActionComponent::ResetActionStates(const map<ActionType, vector<string>>& actionClips)
{
	for (auto pair : actions)
	{
		pair.second->ResetActions(actionClips.at(pair.first));
	}
}

void ActionComponent::SetSkillStateSkill(Skill* skill)
{
	if (skill == nullptr) return;
	((SkillState*)actions[ActionType::SKILL])->SetSkill(skill);
}
