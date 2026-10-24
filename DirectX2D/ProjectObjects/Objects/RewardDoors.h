#pragma once

class AnimObject;

//보상 방 입구: 벽 + 테이블 + 문 2개
//문마다 RewardType 하나 - None이면 파괴된 문(Terminal_Destroyed)이라 들어갈 수 없음
class RewardDoors
{
public:
	RewardDoors();
	~RewardDoors();

	void Update();
	void Render();

	Vector2& Pos();	//벽 위치 - 나머지는 벽의 자식이라 같이 움직임

	void Open();	//스테이지 클리어 시 호출 - 파괴되지 않은 문만 Activate

	RewardType GetLeftReward() { return leftReward; }
	RewardType GetRightReward() { return rightReward; }

private:
	void Init();
	void CreateDoorAnim(AnimObject* door);
	void SetDoorType();
	void PlayClosed(AnimObject* door, RewardType reward, Vector2 pos);

	string GetGateTag(RewardType reward);

private:
	AnimObject* leftDoor = nullptr;
	AnimObject* rightDoor = nullptr;
	Quad* gateWall = nullptr;
	Quad* gateTable = nullptr;
	Quad* gateChoiceTable = nullptr;

	RewardType leftReward = RewardType::None;
	RewardType rightReward = RewardType::None;
};
