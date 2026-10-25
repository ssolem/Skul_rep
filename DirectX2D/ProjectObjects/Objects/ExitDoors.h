#pragma once

class AnimObject;

//문 하나의 정보 
struct DoorInfo
{
	bool destroyed = false;				
	RoomType room = RoomType::Combat;
	RewardType reward = RewardType::None;	
};

class ExitDoors
{
public:
	ExitDoors();
	~ExitDoors();

	void Update();
	void Render();

	Vector2& Pos() { return root->Pos(); }
	void SetGroundPos(Vector2 groundPos) { root->Pos() = groundPos; }

	void Setup(const vector<DoorInfo>& doors, bool withFrame);
	void Open();

	const vector<DoorInfo>& GetDoors() { return infos; }

	//{ 파괴, Gold, Skul, Item } 중에서 중복 없이 2개 - 일반 보상 문
	static vector<DoorInfo> RandomRewardDoors();

	int GetEnterDoor(Collider* collider);
	bool IsOpen() { return isOpen; }
	Vector2 RewardPos() { return gateTable->GlobalPos() + Vector2(0.0f, gateTable->GetSize().y * 0.5f * gateTable->GlobalScale().y); }

private:
	void CreateDoorAnim(AnimObject* door);
	void PlayDoor(int index, bool isOpen);

	string GetGateTag(const DoorInfo& info);

private:
	static const int MAX_DOORS = 2;

	const Vector2 FRAME_DOOR_OFFSET = Vector2(150.0f, -65.0f);
	const Vector2 TABLE_POS = Vector2(0.0f, -55.0f);				
	const float DOOR_GAP = 100.0f;	

	Transform* root = nullptr;	
	Quad* gateWall = nullptr;
	Quad* gateTable = nullptr;
	AnimObject* doorObjects[MAX_DOORS] = {};
	RectCollider* doorColliders[MAX_DOORS] = {};
	Vector2 doorSize;
	bool isOpen = false;

	vector<DoorInfo> infos;
	bool withFrame = false;
};
