#include "Framework.h"
#include "Player.h"
#include "Animator.h"
#include "ActionComponent.h"
#include "SkulClass.h"
#include "Stat.h"
#include "Skuls/BasicSkul.h"
#include "Skuls/ThiefSkul.h"

Player::Player()
	: Character()
{
	SetTag("Player");
	SetPlayer();
	SetupInput();
	SetStats();		//플레이어 공통 기본 스탯 (스컬과 무관)

	SkulClass* thiefSkul = new ThiefSkul();
	skuls.push_back(thiefSkul);
	thiefSkul->Init(this, animator);

	SkulClass* basicSkul = new BasicSkul();
	skuls.push_back(basicSkul);
	basicSkul->Init(this, animator);	//스컬별 추가 스탯이 SetStats()에서 들어갈 수 있으므로 그 뒤에 체력 초기화
	
	SetMaxHP();
	SetCurHP(GetMaxHP());

	PlayClip("Idle");

	InitBody();
	collider->SetTag("PlayerCollider");

	CreateEffects();
	actionComponent = new ActionComponent();
	actionComponent->CreateActionStates(this, thiefSkul->GetActionClipNames(), thiefSkul->GetComboAttacks(), thiefSkul->GetJumpAttack(), thiefSkul->GetBasicAttackDamageType());
	actionComponent->SetSwitchStateSkill(thiefSkul->GetSwitchSkill());
}

void Player::SetStats()
{
	stat->AddStat({ StatType::BaseStat, Attribute::Health, 100.0f, this });
	stat->AddStat({ StatType::BaseStat, Attribute::MovementSpeed, 350.0f, this });
}

Player::~Player()
{
	delete actionComponent;
	delete controller;

	for (SkulClass* skul : skuls)
		delete skul;
}

void Player::PlayClip(const string& name)
{
	animator->PlayClip(skuls[curSkulIndex]->GetTag() + name);
}

Clip* Player::FindClip(const string& name)
{
	return animator->FindClip(skuls[curSkulIndex]->GetTag() + name);
}


//Skill* Player::GetCurSkill()
//{
//	return skuls[curSkulIndex]->GetCurSkill();
//}

void Player::UpdateControl()
{
	actionComponent->UpdateCoolDown();

	if (isKnockBack)
	{
		KnockBack();
		Move();		
		return;
	}

	controller->Update();

	if (isPlayingAction)
	{
		actionComponent->Update();
		if (actionComponent->GetCurActionState() == ActionType::JUMP_ATTACK)
			Move();
		else
			ResetMove();	// 액션 중에도 dir을 매 프레임 초기화 → GetDirection()이 "지금 눌린 방향"만 반영
	}
	else
		Move();

}

void Player::PostUpdate()
{
	UpdatePreInput();
	skuls[curSkulIndex]->Update();	//isPlayingAction과 무관하게 매 프레임 갱신
}

void Player::Render()
{
	Character::Render();
	skuls[curSkulIndex]->Render();	//Update()와 동일한 이유로 항상 렌더링
	actionComponent->Render();
}

void Player::InitBody()
{
	Character::InitBody();

	collider->Pos() += { 0.0f, defaultSize.y * 0.5f };
	collider->Scale().x = 0.55f;
	collider->UpdateWorld();

	groundSensor->Scale() = { 0.55f, 0.05f };
	groundSensor->Pos() += { 0.0f, 0.0f };
	groundSensor->UpdateWorld();

}

void Player::OnHit(const HitData& hit)
{
	Character::OnHit(hit);
}

void Player::TakeDamage(int amount)
{
	Character::TakeDamage(amount);
}

void Player::Die()
{
}

void Player::StartKnockBack(const HitData& hit)
{
	Character::StartKnockBack(hit);

	// 진행 중이던 액션 강제 종료 (공격 판정이 켜진 채 넉백되는 것 방지)
	if (isPlayingAction)
		CancelAction();

	// 맞기 직전 선입력은 버림
	preInput.canUse = false;
}

void Player::EndKnockBack()
{
	Character::EndKnockBack();
}

void Player::TryAction(ActionType type)
{
	if (CanAction(type))
	{
		DoAction(type);
	}
	else
	{
		preInput = { type, preInputTime, GetInputDirx(), true };
	}
}

void Player::DoAction(ActionType type)
{
	if (isPlayingAction)
		CancelAction();

	if (type == ActionType::JUMP)
	{
		DoJump();
		return;
	}

	//교체 먼저 - SWITCH 클립은 새 스컬 것
	if (type == ActionType::SWITCH)
		SwitchSkul();

	OnStartAction();

	actionComponent->Start(type);
}

//빠지는 스컬 정리 → 인덱스 교체 → 액션 상태를 새 스컬 데이터로 교체
void Player::SwitchSkul()
{
	skuls[curSkulIndex]->OnSwitchOut();
	curSkulIndex = (curSkulIndex + 1) % (int)skuls.size();

	SkulClass* skul = skuls[curSkulIndex];
	actionComponent->ResetActionStates(skul->GetActionClipNames(), skul->GetComboAttacks(), skul->GetJumpAttack(), skul->GetBasicAttackDamageType());
	actionComponent->SetSwitchStateSkill(skul->GetSwitchSkill());

	refreshPlayerInfo();
}

float Player::GetSwitchCoolDownRatio()
{
	return actionComponent->GetSwitchCoolDownRatio();
}

void Player::CancelAction()
{
	actionComponent->End();
}

bool Player::CanAction(ActionType type)
{
	if (isKnockBack) return false;
	if (type == ActionType::SWITCH && GetSubSkul() == nullptr) return false;	//스컬 하나면 교체 불가

	if (type == ActionType::JUMP)
	{
		if (!isPlayingAction) return true;
		return actionComponent->CanJumpCancel();
	}

	if (isPlayingAction && !canCancelAction)
	{
		if (!(actionComponent->GetCurActionState() == ActionType::DASH && type == ActionType::DASH))
			return false;
	}

	return actionComponent->IsReady(type);
}

void Player::UpdatePreInput()
{
	if (!preInput.canUse) return;

	preInput.remainTime -= DELTA;
	if (preInput.remainTime <= 0.0f)
	{
		preInput.canUse = false;
		return;
	}

	if (CanAction(preInput.type))
	{
		preInput.canUse = false;
		DoAction(preInput.type);
	}
}

void Player::DoJump()
{
	if (isGrounded && groundFloor != nullptr && KEY_PRESS(VK_DOWN))
	{
		isFalling = true;
		dropThrough = groundFloor;
		Fall();
		return;
	}

	bool wasGrounded = isGrounded;

	if (!Jump()) return;    

	if (!wasGrounded)      
		EffectManager::Get()->Play("DoubleJump_Smoke", GlobalPos() + jumpEffectOffset);
}


void Player::OnJumpPressed()
{
	if (isPlayingAction)
	{
		if (actionComponent->CanJumpCancel())
		{
			CancelAction();
			DoJump();
		}
		else if (actionComponent->CanPreInputJump())
		{
			preInput = { ActionType::JUMP, preInputTime, 0.0f, true };
		}

		return;
	}

	DoJump();
}

void Player::DoSkill1()
{
	actionComponent->SetSkillStateSkill(skuls[curSkulIndex]->GetCurSkill()[0]);
	TryAction(ActionType::SKILL);
}

void Player::DoSkill2()
{
	actionComponent->SetSkillStateSkill(skuls[curSkulIndex]->GetCurSkill()[1]);
	TryAction(ActionType::SKILL);
}

float Player::GetInputDirx()
{
	float x = 0.0f;
	if (KEY_PRESS(VK_RIGHT)) x += 1.0f;
	if (KEY_PRESS(VK_LEFT)) x -= 1.0f;

	return x;
}


void Player::SetupInput()
{
	controller = new ObjectController(this);

	//Walk
	controller->AddInput("MoveRight", VK_RIGHT, InputAction::InputType::PRESS, nullptr);
	controller->BindAction("MoveRight", [this](void*) { MoveVector2(1, 0); });
	controller->AddInput("MoveLeft", VK_LEFT, InputAction::InputType::PRESS, nullptr);
	controller->BindAction("MoveLeft", [this](void*) { MoveVector2(-1, 0); });

	//Jump
	controller->AddInput("Jump", 'C', InputAction::InputType::DOWN, nullptr);
	controller->BindAction("Jump", [this](void*) { OnJumpPressed(); });

	//Attack
	controller->AddInput("Attack", 'X', InputAction::InputType::DOWN, nullptr);
	controller->BindAction("Attack", [this](void*) { TryAction(isGrounded ? ActionType::BASIC_ATTACK : ActionType::JUMP_ATTACK); });

	//Dash
	controller->AddInput("Dash", 'Z', InputAction::InputType::DOWN, nullptr);
	controller->BindAction("Dash", [this](void*) { TryAction(ActionType::DASH); });

	//Switch
	controller->AddInput("Switch", VK_SPACE, InputAction::InputType::DOWN, nullptr);
	controller->BindAction("Switch", [this](void*) { TryAction(ActionType::SWITCH); });

	//Skill
	controller->AddInput("Skill1", 'A', InputAction::InputType::DOWN, nullptr);
	controller->BindAction("Skill1", [this](void*) { DoSkill1(); });

	controller->AddInput("Skill2", 'S', InputAction::InputType::DOWN, nullptr);
	controller->BindAction("Skill2", [this](void*) { DoSkill2(); });

}

void Player::CreateEffects()
{
	animator->CreateEffectByAtlas("DashSmoke_Public_Small", L"ProjectTextures/Effects/DashSmoke_Public_Small/DashSmoke_Public_Small.png", 4, SKUL_DEFAULT_ASSET_SIZE);
	animator->CreateEffectByAtlas("DoubleJump_Smoke", L"ProjectTextures/Effects/DoubleJump_Smoke/DoubleJump_Smoke.png", 5, SKUL_DEFAULT_ASSET_SIZE);
	animator->CreateEffectByAtlas("Enemy_Appearance", L"ProjectTextures/Effects/Enemy_Appearance/Enemy_Appearance.png", 20, SKUL_DEFAULT_ASSET_SIZE);
	SetEffectDetails();
}

void Player::SetEffectDetails()
{
	//Jump
	jumpEffectOffset = { 0.0f, defaultSize.y * 0.5f };
}
