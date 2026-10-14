#include "Framework.h"
#include "StageBackground.h"

StageBackground::StageBackground()
{
	camPrevPos = CAM->Pos();
}

StageBackground::~StageBackground()
{
	for(Layer& layer : layers)
	{
		for (Quad* copy : layer.copies)
		{
			delete copy;
		}
	}
	layers.clear();
}

void StageBackground::Update()
{
	Vector2 diff = CAM->Pos() - camPrevPos;
	camPrevPos = CAM->Pos();

	float camCenterX = CAM->Pos().x + CAM->deviceSize.x * 0.5f - GlobalPos().x;

	for (Layer& layer : layers)
	{
		layer.offsetX += diff.x * layer.moveSpeed;
		layer.offsetY -= diff.y * abs(layer.moveSpeed) * 0.1f;

		float rel = camCenterX - layer.offsetX;
		int centerIndex = (int)floorf(rel / layer.width + 0.5f);

		for (int i = 0; i < 3; ++i)
		{
			int index = centerIndex + i - 3 / 2;
			layer.copies[i]->Pos() = { layer.offsetX + index * layer.width, layer.offsetY };
			layer.copies[i]->UpdateWorld();
		}
	}
}

void StageBackground::Render()
{
	for(Layer& layer : layers)
	{
		for (Quad* copy : layer.copies)
		{
			copy->Render();
		}
	}	
}

void StageBackground::AddLayer(wstring file, float moveSpeed)
{
	Layer layer;
	layer.moveSpeed = moveSpeed;

	for(int i = 0; i < 3; ++i)
	{
		Quad* copy = new Quad(file);
		copy->SetParent(this);
		copy->Scale() = SKUL_DEFAULT_ASSET_SIZE;
		layer.copies.push_back(copy);
	}
	layer.width = layer.copies[0]->GetSize().x * SKUL_DEFAULT_ASSET_SIZE.x;
	layers.push_back(layer);
}

void StageBackground::Reset()
{
	camPrevPos = CAM->Pos();
	for (Layer& layer : layers)
		layer.offsetX = layer.offsetY = 0.0f;
}