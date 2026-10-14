#include "Framework.h"
#include "TestCharacterScene.h"
#include "ProjectObjects/Character/Player.h"
#include "ProjectObjects/Character/Enemy/Normal/Enemy.h"
#include "ProjectObjects/Character/Enemy/Normal/Ent.h"
#include "ProjectObjects/Character/Enemy/Normal/RootEnt.h"
#include "ProjectObjects/Character/Enemy/Normal/GiganticEnt.h"
#include "ProjectObjects/Character/Enemy/Normal/BlossomEnt.h"
#include "ProjectObjects/Stage.h"
#include "ProjectObjects/UI/PlayerInfoUI.h"
#include "ProjectObjects/StageBackground.h"


TestCharacterScene::TestCharacterScene()
{
	player = new Player();
	player->Pos() = { CENTER_X, 1010.0f };

	CAM->SetTarget(player);

	stage = new Stage("ProjectMaps/Chapter1_02.map");

	player->SetStage(stage);
	stage->SetPlayer(player);

	//enemy 임시
	ent = new Ent();
	ent->Pos() = { CENTER_X + 200.0f , 1010.0f };
	stage->RegisterEnemy(ent);

	ent2 = new RootEnt();
	ent2->Pos() = { CENTER_X + 100.0f , 1010.0f };
	stage->RegisterEnemy(ent2);

	ent3 = new GiganticEnt();
	ent3->Pos() = { CENTER_X + 100.0f , 1010.0f };
	stage->RegisterEnemy(ent3);

	ent4 = new BlossomEnt();
	ent4->Pos() = { CENTER_X + 300.0f , 1010.0f };
	stage->RegisterEnemy(ent4);

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

	ent->Spawn(ent->Pos());
	ent2->Spawn(ent2->Pos());
	ent3->Spawn(ent3->Pos());
	ent4->Spawn(ent4->Pos());
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

	EffectManager::Get()->Update();
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
