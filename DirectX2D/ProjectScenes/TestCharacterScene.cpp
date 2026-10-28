#include "Framework.h"
#include "TestCharacterScene.h"
#include "ProjectObjects/Character/Player.h"
#include "ProjectObjects/Stage.h"
#include "ProjectObjects/StageData.h"
#include "ProjectObjects/WaveSpawner.h"
#include "ProjectObjects/UI/PlayerInfoUI.h"
#include "ProjectObjects/StageBackground.h"
#include "ProjectObjects/Objects/GoldItem.h"
#include "ProjectObjects/Objects/BoneItem.h"
#include "ProjectObjects/Objects/DarkQuartzItem.h"
#include "ProjectObjects/Objects/StageReward.h"


TestCharacterScene::TestCharacterScene()
{
	player = new Player();	//"Enemy_Appearance" 이펙트를 등록하므로 적 생성(Build)보다 먼저

	playerInfoUI = new PlayerInfoUI(player);
	playerInfoUI->Scale() = SKUL_DEFAULT_ASSET_SIZE;
	playerInfoUI->Pos() = Vector2(168.0f, 66.0f);

	player->SetOnHPChanged([this](float curHP, float maxHP)
		{
			playerInfoUI->SetMaxHP(maxHP);
			playerInfoUI->SetHP(curHP);
		});
	player->SetRefreshPlayerInfo([this]() {playerInfoUI->Refresh(); });

	background = new StageBackground();
	background->AddLayer(L"ProjectTextures/Backgrounds/Chapter1/layer03.png", 0.0f);
	background->AddLayer(L"ProjectTextures/Backgrounds/Chapter1/layer02.png", 0.05f);
	background->AddLayer(L"ProjectTextures/Backgrounds/Chapter1/layer01.png", -0.1f);
	background->AddLayer(L"ProjectTextures/Backgrounds/Chapter1/layer00.png", 0.2f);

	exitDoors = new ExitDoors();

	//보상
	stageReward = new StageReward();
	stageReward->SetOnOpened([this](RewardType reward)
		{
			SpawnReward(reward);
			OpenExitDoors();
		});

	//스테이지 등록
	for (int i = 1; i <= 23; i++)
	{
		char name[64];
		sprintf_s(name, "ProjectMaps/Chapter1_%02d.stage.json", i);
		stageFiles[RoomType::Combat].push_back(name);
	}

	//json 만들면 주석 해제 (.map만 있고 적/문 위치가 없음)
	//stageFiles[RoomType::Elite] = { "ProjectMaps/Chapter1_elite.stage.json" };
	//stageFiles[RoomType::Boss] = { "ProjectMaps/Chapter1_boss.stage.json" };
	//stageFiles[RoomType::Shop] = { "ProjectMaps/Chapter1_shop.stage.json" };
}

TestCharacterScene::~TestCharacterScene()
{
	delete stageReward;
	delete exitDoors;
	delete playerInfoUI;
	delete stage;		
	delete background;	
	delete player;
}

//json을 읽어 맵/플레이어 위치/웨이브를 새로 구성
void TestCharacterScene::BuildStage()
{
	StageData stageData;
	string file = GetStageFile(curDoor.room);
	if (file.empty() || !StageDataIO::Load(file, stageData))
	{
		stageData = StageData();
		if (StageDataIO::Load(stageFiles[RoomType::Combat][0], stageData))
		{
			stageData.waves.clear();
		}
		else
		{
			stageData.mapFile = "ProjectMaps/Chapter1_02.map";
			stageData.playerSpawn = { CENTER_X, 1010.0f };
		}
	}

	stage = new Stage(stageData.mapFile);

	player->SetStage(stage);
	stage->SetPlayer(player);
	player->Pos() = stage->GetGroundPos(stageData.playerSpawn);	//Pos()가 발 위치라 바닥에 바로 세움

	hasExit = stageData.hasDoor;
	if (hasExit)
	{
		exitDoors->SetGroundPos(stageData.door);
		SetExitDoors();
	}

	stageReward->Setup(RewardType::None, Vector2());	//이전 방 보상 숨김

	WaveSpawner* spawner = stage->GetWaveSpawner();
	spawner->Build(stageData);
	spawner->SetOnClear([this]()
		{
			if (!hasExit) return;

			//보상 없는 방(첫 방, 상점 출구 등)은 바로 문 열기
			if (curDoor.reward == RewardType::None)
			{
				OpenExitDoors();
				return;
			}

			//보상 있는 방은 상자를 열어야 문이 열림 (SetOnOpened)
			stageReward->Setup(curDoor.reward, exitDoors->RewardPos());
		});
	spawner->Start();

	stage->SetBackgroundOffset(Vector2(0.0f, 90.0f));
	background->Pos() = stage->GetBackgroundPos();
	background->UpdateWorld();
	stage->SetBackground(background);
}

//적/아이템/타일맵을 지움 - 등장 중인 적의 이펙트 콜백은 Enemy 소멸자가 끊음
void TestCharacterScene::ClearStage()
{
	player->SetStage(nullptr);

	delete stage;
	stage = nullptr;
}

void TestCharacterScene::SetExitDoors()
{
	RoomType nextRoom = GetNextRoom();

	if (nextRoom != RoomType::Combat)
	{
		DoorInfo door;
		door.room = nextRoom;
		exitDoors->Setup({ door }, false);
		return;
	}

	if (curDoor.room == RoomType::Shop)
	{
		DoorInfo door;
		exitDoors->Setup({ door, door }, false);
		return;
	}

	exitDoors->Setup(ExitDoors::RandomRewardDoors(), true);
}

void TestCharacterScene::OpenExitDoors()
{
	exitDoors->Open();
	exitDoors->Update();
}

//보상 오브젝트가 열린 뒤 실제 드랍
void TestCharacterScene::SpawnReward(RewardType reward)
{
	Vector2 spawnPos = stageReward->Pos() + Vector2(0.0f, 80.0f);

	switch (reward)
	{
	case RewardType::Gold:
		for (int i = 0; i < 5; i++)
			stage->SpawnItem(new GoldItem(10), spawnPos);
		break;
	case RewardType::Skul:
		//1. thief 머리 없을때 thief 머리 아이템 뿌리기.
		//2. 있으면 skulbone 뿌리기
		break;
	case RewardType::Item:
		//아직 아이템이 없음 - darkquartz 뿌리기
		for (int i = 0; i < 4; i++)
			stage->SpawnItem(new DarkQuartzItem(1), spawnPos);
		break;
	}
}

void TestCharacterScene::ChangeStage()
{
	if (!changeStage) return;
	changeStage = false;

	ClearStage();

	roomCount++;
	curDoor = nextDoor;

	BuildStage();
	stage->SetCameraBounds();
}

RoomType TestCharacterScene::GetNextRoom()
{
	auto it = chapterOne.find(roomCount);
	if (it != chapterOne.end())
		return it->second;

	return RoomType::Combat;
}

void TestCharacterScene::ResetStagePool()
{
	combatPool = stageFiles[RoomType::Combat];

	for (int i = (int)combatPool.size() - 1; i > 0; i--)
		swap(combatPool[i], combatPool[Random(0, i + 1)]);
}

string TestCharacterScene::GetStageFile(RoomType room)
{
	if (room == RoomType::Combat)
	{
		if (combatPool.empty()) ResetStagePool();

		string file = combatPool.back();
		combatPool.pop_back();
		return file;
	}

	vector<string>& files = stageFiles[room];
	if (files.empty()) return "";

	return files[Random(0, (int)files.size())];
}

void TestCharacterScene::Update()
{
	player->UpdateWorld();
	player->Update();
	stage->Update();	//등록된 스컬 오브젝트(head 등)를 액션 상태와 무관하게 매 프레임 갱신

	if (hasExit)
		exitDoors->Update();

	stageReward->Update();

	if (KEY_DOWN(VK_F2))
	{
		SceneManager::Get()->Add("Atlas Scene");
		SceneManager::Get()->Remove("TestCharacter Scene");
	}
	if (KEY_DOWN(VK_F3))
	{
		SceneManager::Get()->Add("SkulMapEdit Scene");
		SceneManager::Get()->Remove("TestCharacter Scene");
	}
	if (KEY_DOWN(VK_F4))
	{
		SceneManager::Get()->Add("SpawnEdit Scene");
		SceneManager::Get()->Remove("TestCharacter Scene");
	}

	if (KEY_DOWN('F'))
	{
		//보상과 문 판정이 겹치면 보상 먼저 (문은 보상을 열어야 열림)
		if (stageReward->IsInteractable(player->GetCollider()))
		{
			stageReward->Open();
		}
		else if (hasExit)
		{
			int index = exitDoors->GetEnterDoor(player->GetCollider());
			if (index >= 0)
			{
				nextDoor = exitDoors->GetDoors()[index];
				nextDoor.room = GetNextRoom();
				changeStage = true;
			}
		}
	}

	//테스트: 플레이어 위에 골드 뿌리기
	if (KEY_DOWN(VK_F5))
	{
		for (int i = 0; i < 5; i++)
			stage->SpawnItem(new GoldItem(10), player->GlobalPos() + Vector2(0.0f, 80.0f));
	}
	//테스트: 뼈 조각 / 다크쿼츠
	if (KEY_DOWN(VK_F6))
	{
		for (int i = 0; i < 4; i++)
			stage->SpawnItem(new BoneItem(1), player->GlobalPos() + Vector2(0.0f, 80.0f));
	}
	if (KEY_DOWN(VK_F7))
	{
		for (int i = 0; i < 4; i++)
			stage->SpawnItem(new DarkQuartzItem(1), player->GlobalPos() + Vector2(0.0f, 80.0f));
	}

	EffectManager::Get()->Update();
	DamageTextManager::Get()->Update();
	playerInfoUI->Update();

	ChangeStage();
}

void TestCharacterScene::Render()
{
	stage->Render();

	if (hasExit)
		exitDoors->Render();

	stageReward->Render();

	player->Render();
	EffectManager::Get()->Render();
}

void TestCharacterScene::PostRender()
{
	DamageTextManager::Get()->Render();
	playerInfoUI->Render();
	playerInfoUI->RenderUI();
	player->PostRender();
}

void TestCharacterScene::Start()
{
	roomCount = 1;
	curDoor = DoorInfo();
	changeStage = false;

	ResetStagePool();
	BuildStage();

	CAM->SetTarget(player);
	stage->SetCameraBounds();
}

void TestCharacterScene::End()
{
	ClearStage();
}

void TestCharacterScene::Reset()
{
}
