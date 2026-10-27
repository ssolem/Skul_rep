#pragma once
class AnimObject;

class StageReward
{
public:
	StageReward();
	~StageReward();

	void Update();
	void Render();

	Vector2& Pos() { return position->Pos(); }

	void Setup(RewardType rewardType, Vector2 pos);
	void Open();
	bool IsInteractable(Collider* player);
private:
	void CreateRewardAnim();
	string GetRewardTag();

private:
	const Vector2 REWARD_COL_OFFSET = Vector2(0.0f, 50.0f);
	const Vector2 REWARD_COL_SIZE = Vector2(100.0f, 100.0f);

	Transform* position = nullptr;
	RewardType type = RewardType::None;
	RectCollider* rewardCollider = nullptr;
	AnimObject* rewardObject = nullptr;

	bool isOpen = false;

};

