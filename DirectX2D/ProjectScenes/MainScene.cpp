#include "Framework.h"
#include "MainScene.h"
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
#include "ProjectObjects/Objects/SkulItem.h"
#include "ProjectObjects/Character/Skuls/ThiefSkul.h"
#include "ProjectObjects/UI/UpgradeUI.h"


MainScene::MainScene()
{
	player = new Player();	//"Enemy_Appearance" 이펙트를 등록하므로 적 생성(Build)보다 먼저

	ThiefSkul().Preload();

	playerInfoUI = new PlayerInfoUI(player);
	playerInfoUI->Scale() = SKUL_DEFAULT_ASSET_SIZE;
	playerInfoUI->Pos() = Vector2(168.0f, 66.0f);

	player->SetOnHPChanged([this](float curHP, float maxHP)
		{
			playerInfoUI->SetMaxHP(maxHP);
			playerInfoUI->SetHP(curHP);
		});
	player->SetRefreshPlayerInfo([this]() {playerInfoUI->Refresh(); });
	player->SetOnDie([this]() { returnHome = true; });

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

	//stageFiles[RoomType::Elite] = { "ProjectMaps/Chapter1_elite.stage.json" };
	//stageFiles[RoomType::Boss] = { "ProjectMaps/Chapter1_boss.stage.json" };
	//stageFiles[RoomType::Shop] = { "ProjectMaps/Chapter1_shop.stage.json" };

	//거점 - 1번 맵과 같은 지형/플레이어/문 위치, 웨이브 없음
	stageFiles[RoomType::Home] = { "ProjectMaps/Chapter1_home.stage.json" };

	upgradeUI = new UpgradeUI(player);
}

MainScene::~MainScene()
{
	delete stageReward;
	delete exitDoors;
	delete playerInfoUI;
	delete upgradeUI;
	delete stage;		
	delete background;	
	delete player;
}

//json을 읽어 맵/플레이어 위치/웨이브를 새로 구성
void MainScene::BuildStage()
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
	player->Teleport(stage->GetGroundPos(stageData.playerSpawn));

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

			//보상 없는 방은 바로 문 열기
			if (curDoor.reward == RewardType::None)
			{
				OpenExitDoors();
				return;
			}

			//보상 있는 방은 상자를 열어야 문이 열림 
			stageReward->Setup(curDoor.reward, exitDoors->RewardPos());
		});
	spawner->Start();

	stage->SetBackgroundOffset(Vector2(0.0f, 90.0f));
	background->Pos() = stage->GetBackgroundPos();
	background->UpdateWorld();
	stage->SetBackground(background);
}

//적/아이템/타일맵을 지움
void MainScene::ClearStage()
{
	stage->AcquireRemainingItems();

	player->OnLeaveStage();
	player->SetStage(nullptr);

	ProjectileManager::Get()->Clear();

	delete stage;
	stage = nullptr;
}

void MainScene::SetExitDoors()
{
	if (curDoor.room == RoomType::Home)
	{
		DoorInfo door;
		door.reward = RewardType::Skul;
		exitDoors->Setup({ door }, false);
		return;
	}

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

void MainScene::OpenExitDoors()
{
	exitDoors->Open();
	exitDoors->Update();
}

//보상 오브젝트가 열린 뒤 실제 드랍
void MainScene::SpawnReward(RewardType reward)
{
	Vector2 spawnPos = stageReward->Pos() + Vector2(0.0f, 80.0f);

	switch (reward)
	{
	case RewardType::Gold:
		for (int i = 0; i < 5; i++)
			stage->SpawnItem(new GoldItem(10), spawnPos);
		break;
	case RewardType::Skul:
		//Thief 없으면 머리, 있으면 뼈
		if (!player->HasSkul<ThiefSkul>())
			stage->SpawnItem(new SkulItem(new ThiefSkul()), spawnPos, stageReward->Pos() + Vector2(150.0f, 0.0f));
		else
		{
			for (int i = 0; i < 4; i++)
				stage->SpawnItem(new BoneItem(10), spawnPos);
		}
		break;
	case RewardType::Item:
		//아직 아이템이 없음 - darkquartz 뿌리기
		for (int i = 0; i < 4; i++)
			stage->SpawnItem(new DarkQuartzItem(10), spawnPos);
		break;
	}
}

void MainScene::GoHome()
{
	returnHome = false;
	changeStage = false;

	ClearStage();

	roomCount = 0;
	curDoor = DoorInfo();
	curDoor.room = RoomType::Home;
	ResetStagePool();

	player->SetCurHP(player->GetMaxHP());

	BuildStage();
	stage->SetCameraBounds();
}

void MainScene::ChangeStage()
{
	if (returnHome)
	{
		GoHome();
		return;
	}

	if (!changeStage) return;
	changeStage = false;

	ClearStage();

	roomCount++;
	curDoor = nextDoor;

	BuildStage();
	stage->SetCameraBounds();
}

RoomType MainScene::GetNextRoom()
{
	auto it = chapterOne.find(roomCount);
	if (it != chapterOne.end())
		return it->second;

	return RoomType::Combat;
}

void MainScene::ResetStagePool()
{
	combatPool = stageFiles[RoomType::Combat];

	for (int i = (int)combatPool.size() - 1; i > 0; i--)
		swap(combatPool[i], combatPool[Random(0, i + 1)]);
}

string MainScene::GetStageFile(RoomType room)
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

void MainScene::Update()
{
	//강화창이 열려 있으면 플레이어 조작/상호작용 막음 (위치 갱신은 유지)
	bool isUIOpen = upgradeUI->IsOpen();

	player->UpdateWorld();
	if (!isUIOpen)
		player->Update();
	stage->Update();	//등록된 스컬 오브젝트(head 등)를 액션 상태와 무관하게 매 프레임 갱신

	if (hasExit)
		exitDoors->Update();

	stageReward->Update();

	if (KEY_DOWN(VK_F2))
	{
		SceneManager::Get()->Add("Atlas Scene");
		SceneManager::Get()->Remove("Main Scene");
	}
	if (KEY_DOWN(VK_F3))
	{
		SceneManager::Get()->Add("SkulMapEdit Scene");
		SceneManager::Get()->Remove("Main Scene");
	}
	if (KEY_DOWN(VK_F4))
	{
		SceneManager::Get()->Add("SpawnEdit Scene");
		SceneManager::Get()->Remove("Main Scene");
	}

	if (!isUIOpen && KEY_DOWN('F'))
	{
		//보상과 문 판정이 겹치면 보상 먼저 (문은 보상을 열어야 열림)
		if (stageReward->IsInteractable(player->GetCollider()))
		{
			stageReward->Open();
		}
		else if (DropItem* item = stage->FindInteractableItem(player->GetCollider()))
		{
			//액션 중엔 교체 안 함
			if (!player->IsPlayingAction())
				item->Interact();
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

	if (KEY_DOWN(VK_F8)/* && curDoor.room == RoomType::Home*/)
		upgradeUI->OpenUI();

	EffectManager::Get()->Update();
	DamageTextManager::Get()->Update();
	playerInfoUI->Update();
	upgradeUI->Update();

	ChangeStage();
}

void MainScene::Render()
{
	stage->RenderMap();

	if (hasExit)
		exitDoors->Render();

	stageReward->Render();

	stage->RenderObjects();

	player->Render();
	EffectManager::Get()->Render();
}

void MainScene::PostRender()
{
	DamageTextManager::Get()->Render();
	playerInfoUI->Render();
	playerInfoUI->RenderUI();
	upgradeUI->Render();
	player->PostRender();
}

void MainScene::Start()
{
	roomCount = 1;
	curDoor = DoorInfo();
	curDoor.reward = RewardType::Skul;	//첫 방은 무덤 보상
	changeStage = false;
	returnHome = false;

	ResetStagePool();
	BuildStage();

	CAM->SetTarget(player);
	stage->SetCameraBounds();
	
	//bgm
	Audio::Get()->Add("BGM_Chapter1", "ProjectSounds/BGM/Chapter1.wav", true);	
	Audio::Get()->Play("BGM_Chapter1");
}

void MainScene::End()
{
	Audio::Get()->Stop("BGM_Chapter1");
	ClearStage();
}

void MainScene::Reset()
{
}
