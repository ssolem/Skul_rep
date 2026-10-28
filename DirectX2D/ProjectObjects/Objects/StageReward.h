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

	void SetOnOpened(function<void(RewardType)> callback) { onOpened = callback; }

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
	bool isRewarded = false;	//비루프 클립은 마지막 프레임 이벤트가 두 번 불려서 드랍 중복 방지

	function<void(RewardType)> onOpened;

};

