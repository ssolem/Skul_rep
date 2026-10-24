#pragma once

class AnimObject;

class RewardDoors
{
public:
	RewardDoors();
	~RewardDoors();

	void Update();
	void Render();

	Vector2& Pos() { return gateWall->Pos(); }
	void SetGroundPos(Vector2 groundPos);

	void Open();

	RewardType GetLeftReward() { return leftReward; }
	RewardType GetRightReward() { return rightReward; }

private:
	void Init();
	void CreateDoorAnim(AnimObject* door);
	void SetDoorType();
	void PlayClosed(AnimObject* door, RewardType reward, Vector2 pos);

	string GetGateTag(RewardType reward);

private:
	const Vector2 LEFT_DOOR_POS = Vector2(-150.0f, -65.0f);
	const Vector2 RIGHT_DOOR_POS = Vector2(150.0f, -65.0f);
	const Vector2 TABLE_POS = Vector2(0.0f, -55.0f);

	AnimObject* leftDoor = nullptr;
	AnimObject* rightDoor = nullptr;
	Quad* gateWall = nullptr;
	Quad* gateTable = nullptr;

	RewardType leftReward = RewardType::None;
	RewardType rightReward = RewardType::None;
};
