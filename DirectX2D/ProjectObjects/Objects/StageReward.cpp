#include "Framework.h"
#include "StageReward.h"
#include "ProjectObjects/Objects/AnimObject.h"

StageReward::StageReward()
{
	position = new Transform();
	position->Scale() = SKUL_DEFAULT_ASSET_SIZE;

	rewardObject = new AnimObject();
	rewardObject->SetParent(position);
	rewardObject->Scale() = Vector2::One;

	CreateRewardAnim();
	rewardObject->Stop();

	rewardCollider = new RectCollider(REWARD_COL_SIZE);
	rewardCollider->SetParent(position);
	rewardCollider->Pos() += REWARD_COL_OFFSET;
	rewardCollider->SetActive(false);
}

StageReward::~StageReward()
{
	delete rewardCollider;
	delete rewardObject;
	delete position;
}

void StageReward::Update()
{
	rewardObject->Update();
}

void StageReward::Render()
{
	rewardObject->Render();

	rewardCollider->Render();
}

void StageReward::Setup(RewardType rewardType, Vector2 pos)
{
	if (rewardType == RewardType::None) 
	{ 
		rewardObject->Stop(); 
		rewardCollider->SetActive(false); 
		return; 
	}

	Pos() = pos;
	isOpen = false;
	isRewarded = false;
	type = rewardType;

	rewardObject->Stop();
	rewardObject->Play(GetRewardTag() + "Activate", rewardObject->Pos());

	position->UpdateWorld();
	rewardCollider->UpdateWorld();
	rewardCollider->SetActive(true);

	//등장 이펙트 - 중앙 기준이라 셀 높이 절반만큼 올려 바닥에 맞춤
	EffectManager::Get()->Play("MapReward_Spawn", pos);
}

void StageReward::Open()
{
	if (isOpen) return;

	isOpen = true;
	rewardCollider->SetActive(false);

	string key = GetRewardTag() + "Deactivate";
	Clip* clip = rewardObject->FindClip(key);

	if (clip == nullptr)
	{
		isRewarded = true;
		if (onOpened) onOpened(type);
		return;
	}

	clip->ClearEvents();
	clip->SetEvent([this]()
		{
			if (isRewarded) return;

			isRewarded = true;
			if (onOpened) onOpened(type);
		});

	rewardObject->Play(key, rewardObject->Pos());
}

bool StageReward::IsInteractable(Collider* player)
{
	if (isOpen || !rewardCollider->Active()) return false;

	return rewardCollider->IsCollision(player);
}

void StageReward::CreateRewardAnim()
{
	wstring path = L"ProjectTextures/Common/Reward/";

	rewardObject->AddClip("Activate", path + L"Chest_Common", false, 1.0f, "ChestCommon");
	rewardObject->AddClip("Deactivate", path + L"Chest_Common", false, 1.0f, "ChestCommon");

	rewardObject->AddClip("Activate", path + L"Grave_Common", false, 1.0f, "GraveCommon");
	rewardObject->AddClip("Deactivate", path + L"Grave_Common", false, 1.0f, "GraveCommon");

	rewardObject->AddClip("Activate", path + L"GoldReward", true, 1.0f, "GoldReward");
	rewardObject->AddClip("Deactivate", path + L"GoldReward", false, 1.0f, "GoldReward");

	rewardObject->AddClip("Activate", path + L"BossChest_Ch1", false, 1.0f, "BossChest");
	rewardObject->AddClip("Deactivate", path + L"BossChest_Ch1", false, 1.0f, "BossChest");
	rewardObject->AddClip("Opened", path + L"BossChest_Ch1", false, 1.0f, "BossChest");
}

string StageReward::GetRewardTag()
{
	switch (type)
	{
	case RewardType::Gold: return "GoldReward";
	case RewardType::Skul: return "GraveCommon";
	case RewardType::Item: return "ChestCommon";
	}

	return "";
}
