#pragma once
#include "ProjectObjects/Objects/ExitDoors.h"

class Player;
class Enemy;
class Stage;
class PlayerInfoUI;
class StageBackground;
class StageReward;
class UpgradeUI;

class MainScene : public Scene
{
public:
	MainScene();
	~MainScene();

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
	void OpenExitDoors();
	void ChangeStage();
	void GoHome();
	RoomType GetNextRoom();

	string GetStageFile(RoomType room);	
	void ResetStagePool();			

private:
	Player* player;
	Stage* stage = nullptr;
	StageBackground* background;
	PlayerInfoUI* playerInfoUI;
	UpgradeUI* upgradeUI;

	ExitDoors* exitDoors;
	bool hasExit = false;
	StageReward* stageReward;

	int roomCount = 1;
	DoorInfo curDoor;
	DoorInfo nextDoor;
	bool changeStage = false;
	bool returnHome = false;	

	map<int, RoomType> chapterOne = {
		//{4, RoomType::Elite},
		//{8, RoomType::Boss},
	};

	map<RoomType, vector<string>> stageFiles;
	vector<string> combatPool;
};

