#include "Framework.h"
#include "TestCharacterScene.h"
#include "ProjectObjects/Character/Player.h"
#include "ProjectObjects/Character/Enemy/Normal/Enemy.h"
#include "ProjectObjects/Character/Enemy/EnemyFactory.h"
#include "ProjectObjects/Stage.h"
#include "ProjectObjects/UI/PlayerInfoUI.h"
#include "ProjectObjects/StageBackground.h"
#include "ProjectObjects/Objects/GoldItem.h"
#include "ProjectObjects/Objects/BoneItem.h"
#include "ProjectObjects/Objects/DarkQuartzItem.h"


TestCharacterScene::TestCharacterScene()
{
	player = new Player();
	player->Pos() = { CENTER_X, 1010.0f };

	CAM->SetTarget(player);

	stage = new Stage("ProjectMaps/Chapter1_02.map");

	player->SetStage(stage);
	stage->SetPlayer(player);

	//enemy 임시 - 스폰 파일(.spawn)이 생기면 이 블록이 통째로 빠짐
	struct TestSpawn { string type; Vector2 pos; };
	TestSpawn testSpawns[] =
	{
		{ "Ent",         { CENTER_X + 200.0f, 1010.0f } },
		{ "RootEnt",     { CENTER_X + 100.0f, 1010.0f } },
		{ "GiganticEnt", { CENTER_X + 100.0f, 1010.0f } },
		{ "BlossomEnt",  { CENTER_X + 300.0f, 1010.0f } },
	};

	for (TestSpawn& spawn : testSpawns)
	{
		Enemy* enemy = EnemyFactory::Create(spawn.type);
		if (enemy == nullptr) continue;

		stage->RegisterEnemy(enemy);
		enemy->Spawn(spawn.pos);
	}

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
