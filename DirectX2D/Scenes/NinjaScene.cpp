#include "Framework.h"
#include "NinjaScene.h"
#include "Objects/Ninja/Ninja.h"

NinjaScene::NinjaScene()
{
	ninja = new Ninja();
}

NinjaScene::~NinjaScene()
{
	delete ninja;
}

void NinjaScene::Update()
{
	ninja->Update();
	CommandManager::Get()->Update();
}

void NinjaScene::Render()
{
	ninja->Render();
}

void NinjaScene::PostRender()
{
	ninja->RenderUI();
}
