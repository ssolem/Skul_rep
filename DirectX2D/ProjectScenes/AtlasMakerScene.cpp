#include "Framework.h"
#include "AtlasMakerScene.h"
#include "ProjectUtilities/AtlasMaker.h"

AtlasMakerScene::AtlasMakerScene()
{
	atlasMaker = new AtlasMaker();

	char path[128];
	GetCurrentDirectoryA(sizeof(path), path);
	projectPath = path;

}

AtlasMakerScene::~AtlasMakerScene()
{
	delete atlasMaker;
	if (sample)
		delete sample;
}

void AtlasMakerScene::Update()
{
	if (KEY_DOWN(VK_F1))
	{
		SceneManager::Get()->Add("TestCharacter Scene");
		SceneManager::Get()->Remove("Atlas Scene");
	}
	if (KEY_DOWN(VK_F3))
	{
		SceneManager::Get()->Add("SkulMapEdit Scene");
		SceneManager::Get()->Remove("Atlas Scene");
	}

	if (hasSample)
		sample->UpdateWorld();
}

void AtlasMakerScene::Render()
{
	if (hasSample)
		sample->Render();
}

void AtlasMakerScene::PostRender()
{
	CreateAtlas();
	CreateEffect();
	Save();
}

void AtlasMakerScene::Start()
{
	CAM->SetTarget(nullptr);
	CAM->Pos() = { 0, 0 };
}

void AtlasMakerScene::End()
{
}

void AtlasMakerScene::Reset()
{
}

void AtlasMakerScene::CreateAtlas()
{
	if (ImGui::Button("Make Anim Atlas"))
		DIALOG->OpenDialog(
			"SelectAnimFile",
			"Select File",
			nullptr,
			""
		);

	if (DIALOG->Display("SelectAnimFile"))
	{
		if (DIALOG->IsOk())
		{
			string file = DIALOG->GetFilePathName();

			atlasMaker->CreateAtlas(ToWString(file));
			if (hasSample)
			{
				delete sample;
				sample = nullptr;
				hasSample = false;
			}
			sample = new Quad(Vector2((float)atlasMaker->GetAtlasWidth(),
				(float)atlasMaker->GetAtlasHeight()));
			sample->SetTexture(atlasMaker->GetTexture());
			sample->Pos() = { CENTER_X, CENTER_Y };
			hasSample = true;
		}

		DIALOG->Close();
	}
}

void AtlasMakerScene::CreateEffect()
{
	if (ImGui::Button("Make Effect Atlas"))
		DIALOG->OpenDialog(
			"SelectEffectFile",
			"Select File",
			nullptr,
			""
		);

	if (DIALOG->Display("SelectEffectFile"))
	{
		if (DIALOG->IsOk())
		{
			string file = DIALOG->GetFilePathName();

			atlasMaker->CreateEffectAtlas(ToWString(file));
			if (hasSample)
			{
				delete sample;
				sample = nullptr;
				hasSample = false;
			}
			sample = new Quad(Vector2((float)atlasMaker->GetAtlasWidth(),
				(float)atlasMaker->GetAtlasHeight()));
			sample->SetTexture(atlasMaker->GetTexture());
			sample->Pos() = { CENTER_X, CENTER_Y };
			hasSample = true;
		}

		DIALOG->Close();
	}
}

void AtlasMakerScene::Save()
{
	if (ImGui::Button("Save"))
		DIALOG->OpenDialog(
			"Save",
			"Save File",
			nullptr,
			""
		);

	if (DIALOG->Display("Save"))
	{
		if (DIALOG->IsOk())
		{
			string file = DIALOG->GetFilePathName();
			file += ".png";
			atlasMaker->SaveAtlasToFile(ToWString(file));
		}

		DIALOG->Close();


		delete sample;
		sample = nullptr;
		hasSample = false;
	}

}