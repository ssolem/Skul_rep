#include "Framework.h"
#include "DijkstraScene.h"
#include "Objects/TestObject/TestTracker.h"

DijkstraScene::DijkstraScene()
{
	dijkstra = new Dijkstra();
	tracker = new TestTracker();
}

DijkstraScene::~DijkstraScene()
{
	delete dijkstra;
	delete tracker;
}

void DijkstraScene::Update()
{
	if (KEY_DOWN(VK_SPACE))
	{
		int start = dijkstra->FindClosestNode(tracker->Pos());
		int end = dijkstra->FindClosestNode(mousePos);

		if (start > -1 && end > -1)
			tracker->GetPath() = dijkstra->GetPath(start, end);
	}

	dijkstra->Update();
	tracker->Update();
}

void DijkstraScene::Render()
{
	dijkstra->Render();
	tracker->Render();
}

void DijkstraScene::PostRender()
{
	tracker->PostRender();
}
