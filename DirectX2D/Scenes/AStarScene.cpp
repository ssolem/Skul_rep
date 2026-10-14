#include "Framework.h"
#include "AStarScene.h"
#include "Objects/Tile/GameTileMap.h"
#include "Objects/TestObject/TestTracker.h"

//덤불을 통과하는 동안에, 비행기가 조금 더 느려지게 하기

AStarScene::AStarScene()
{
	map = new GameTileMap("TextData/map1.map", { 80, 80 });

	aStar = new AStar();
	aStar->ImportNodes(map);

	//tracker = new TestTracker(true); //노드 사용 트래커
	//tracker->Pos() = { 80, 80 };
	//tracker->Scale() *= 0.5f;
}

AStarScene::~AStarScene()
{
	delete aStar;
	delete map;
	//delete tracker;
}

void AStarScene::Update()
{
	if (KEY_DOWN(VK_F1))
	{
		SceneManager::Get()->Remove("AStar Scene");
		SceneManager::Get()->Add("Tank Scene");
	}

	if (KEY_DOWN(VK_LBUTTON))
	{
		int start = aStar->FindClosestNode(tracker->Pos());
		int end = aStar->FindClosestNode(mousePos);

		aStar->GetPath(start, end, tracker->GetPath());
	}

	map->PushObject(tracker->GetCollider());
	aStar->Update();
	tracker->Update();
}

void AStarScene::Render()
{
	map->Render();
	aStar->Render();
	tracker->Render();
}

void AStarScene::PostRender()
{
	tracker->PostRender();
}

void AStarScene::Start()
{
	tracker = new TestTracker(true); //노드 사용 트래커
	tracker->Pos() = { 80, 80 };
	tracker->Scale() *= 0.5f;

}

void AStarScene::End()
{
	delete tracker;
}

void AStarScene::Reset()
{
	End();
	Start();
}
