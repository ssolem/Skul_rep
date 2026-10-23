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


TestCharacterScene::TestCharacterScene()
{
	player = new Player();	//"Enemy_Appearance" 이펙트를 등록하므로 적 생성(Build)보다 먼저

	CAM->SetTarget(player);

	//스테이지 데이터 - 읽기 실패하면 적 없이 기본 맵으로
	StageData stageData;
	if (!StageDataIO::Load("ProjectMaps/Chapter1_02.stage.json", stageData))
	{
		stageData.mapFile = "ProjectMaps/Chapter1_02.map";
		stageData.playerSpawn = { CENTER_X, 1010.0f };
	}

	stage = new Stage(stageData.mapFile);

	player->SetStage(stage);
	stage->SetPlayer(player);
	player->Pos() = stage->GetGroundPos(stageData.playerSpawn);	//Pos()가 발 위치라 바닥에 바로 세움

	WaveSpawner* spawner = stage->GetWaveSpawner();
	spawner->Build(stageData);
	spawner->SetOnClear([]()
		{
			//TODO: 문 열기, 보상 드랍
			OutputDebugStringA("[Stage] 클리어\n");
		});
	spawner->Start();

	playerInfoUI = new PlayerInfoUI(player);
	playerInfoUI->Scale() = SKUL_DEFAULT_ASSET_SIZE;
	playerInfoUI->Pos() = Vector2(168.0f, 66.0f);

	player->SetOnHPChanged([this](float curHP, float maxHP)
		{
			playerInfoUI->SetMaxHP(maxHP);
			playerInfoUI->SetHP(curHP);
		});

	background = new StageBackground();
	background->AddLayer(L"ProjectTextures/Backgrounds/Chapter1/layer03.png", 0.0f);
	background->AddLayer(L"ProjectTextures/Backgrounds/Chapter1/layer02.png", 0.05f);
	background->AddLayer(L"ProjectTextures/Backgrounds/Chapter1/layer01.png", -0.1f);
	background->AddLayer(L"ProjectTextures/Backgrounds/Chapter1/layer00.png", 0.2f);
	stage->SetBackgroundOffset(Vector2(0.0f, 90.0f));
	background->Pos() = stage->GetBackgroundPos();
	background->UpdateWorld();
	stage->SetBackground(background);
}

TestCharacterScene::~TestCharacterScene()
{
	delete playerInfoUI;
	delete stage;
	delete player;
}

void TestCharacterScene::Update()
{
	player->UpdateWorld();
	player->Update();
	stage->Update();	//등록된 스컬 오브젝트(head 등)를 액션 상태와 무관하게 매 프레임 갱신

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
}

void TestCharacterScene::Render()
{
	stage->Render();

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
	CAM->SetTarget(player);
	stage->SetCameraBounds();
}

void TestCharacterScene::End()
{
}

void TestCharacterScene::Reset()
{
}
