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

	string GetStageFile(RoomType room);	
	void ResetStagePool();			

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

	//엘리트/보스/상점 맵이 아직 없어서 비워 둠
	map<int, RoomType> chapterOne = {
		//{4, RoomType::Elite},
		//{8, RoomType::Boss},
	};

	map<RoomType, vector<string>> stageFiles;
	vector<string> combatPool;
};

