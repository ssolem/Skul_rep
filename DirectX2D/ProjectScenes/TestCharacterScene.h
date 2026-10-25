#pragma once
#include "ProjectObjects/Objects/ExitDoors.h"

class Player;
class Enemy;
class Stage;
class PlayerInfoUI;
class StageBackground;

class TestCharacterScene : public Scene
{
public:
	TestCharacterScene();
	~TestCharacterScene();

	virtual void Update() override;
	virtual void Render() override;
	virtual void PostRender() override;

	virtual void Start() override;
	virtual void End() override;
	virtual void Reset() override;

private:
	void BuildStage();
	void ClearStage();

	void SetExitDoors();
	void SpawnReward(RewardType reward);
	void ChangeStage();
	RoomType GetNextRoom();

private:
	Player* player;
	Stage* stage = nullptr;
	StageBackground* background;
	PlayerInfoUI* playerInfoUI;

	ExitDoors* exitDoors;
	bool hasExit = false;

	int roomCount = 1;
	DoorInfo curDoor;
	DoorInfo nextDoor;
	bool changeStage = false;

	map<int, RoomType> chapterOne = { {4, RoomType::Shop}, {8, RoomType::Boss} };
};

