#include "Framework.h"
#include "LineScene.h"

LineScene::LineScene()
{
	Vector2 start = { CENTER_X * 0.5f, CENTER_Y };
	Vector2 end = { CENTER_X * 1.5f, CENTER_Y };

	line = new LineCollider(start, end);
}

LineScene::~LineScene()
{
	delete line;
}

void LineScene::Update()
{
	if (line->IsPointCollision(mousePos))
	{
		line->GetColor() = { 1, 0, 0, 1 };
	}
	else
	{
		line->GetColor() = { 0, 1, 1, 1 };
	}

	line->UpdateWorld();
}

void LineScene::Render()
{
	line->Render();
}

void LineScene::PostRender()
{
}
