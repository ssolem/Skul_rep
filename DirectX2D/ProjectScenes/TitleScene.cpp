#include "Framework.h"
#include "TitleScene.h"

TitleScene::TitleScene()
{
	background = new Quad(L"ProjectTextures/UI/Title/Background/Title_Art.png");
	background->Pos() = { CENTER_X, CENTER_Y };
	background->UpdateWorld();

	logo = new Quad(L"ProjectTextures/UI/Title/Logo/Title_Logo.png");
	logo->Pos() = { CENTER_X,  WIN_HEIGHT * 0.4f };
	logo->UpdateWorld();
}

TitleScene::~TitleScene()
{
	delete background;
	delete logo;
}

void TitleScene::Update()
{
	time += DELTA;

	if (IsAnyKeyDown())
	{
		SceneManager::Get()->Add("TestCharacter Scene");
		SceneManager::Get()->Remove("Title Scene");
	}
}

void TitleScene::Render()
{
}

void TitleScene::PostRender()
{
	background->Render();
	logo->Render();

	float opacity = (sin(time * blinkSpeed) + 1.0f) * 0.5f;
	Font::Get()->RenderText("아무 키나 누르세요", { CENTER_X, WIN_HEIGHT * 0.2f }, { 400.0f, 30.0f },
		"Default", "White", opacity);
}

void TitleScene::Start()
{
	time = 0.0f;
	CAM->SetTarget(nullptr);
}

void TitleScene::End()
{
}

void TitleScene::Reset()
{
}

bool TitleScene::IsAnyKeyDown()
{
	for (UINT key = 0; key < 255; key++)
	{
		if (!KEY_DOWN(key)) continue;
		if (IsExceptionKey(key)) continue;

		return true;
	}

	return false;
}

bool TitleScene::IsExceptionKey(UINT key)
{
	if (key >= VK_F1 && key <= VK_F12) return true;
	if (key == 'P' || key == 'O') return true;

	return false;
}
