#include "Framework.h"
#include "SkulMapEditScene.h"
#include "ProjectObjects/Tile/SkulEditTileMap.h"
#include "ProjectObjects/Tile/SkulMapGrid.h"

SkulMapEditScene::SkulMapEditScene()
{
	grid = new SkulMapGrid();
	editTileMap = new SkulEditTileMap();
	editTileMap->SetTileScale(Vector2(2.0f, 2.0f));

	ResetSampleFolder();

	char path[128];
	GetCurrentDirectoryA(sizeof(path), path);
	projectPath = path;

	editTileMap->SetTileSize(Texture::Add(L"ProjectTextures/Tile/Chapter1/0_Tile01_0.png")->GetSize());
	Vector2 renderSize = editTileMap->GetRenderSize();
	selectedSample = new Quad(renderSize);

	grid->CreateGrid(renderSize);
}

SkulMapEditScene::~SkulMapEditScene()
{
	delete grid;
	delete editTileMap;

	delete selectedSample;
}

void SkulMapEditScene::Update()
{
	SampleTileControls();
	ZoomControl();

	grid->Update();
	editTileMap->Update();

	if (KEY_DOWN(VK_F2))
	{
		SceneManager::Get()->Add("Atlas Scene");
		SceneManager::Get()->Remove("SkulMapEdit Scene");
	}
	if (KEY_DOWN(VK_F1))
	{
		SceneManager::Get()->Add("TestCharacter Scene");
		SceneManager::Get()->Remove("SkulMapEdit Scene");
	}
}

void SkulMapEditScene::Render()
{
	grid->Render();
	editTileMap->Render();

	if (selectedSample && selectedSample->GetTexture())
		selectedSample->Render();
}

void SkulMapEditScene::PostRender()
{
	editTileMap->SelectBaseTile();

	const char* list[] = { "BG", "FLOOR", "STAIR", "WALL"};

	ImGui::ListBox("Type", (int*)&selectType, list, 4);

	OpenSamples();

	Save();
	Load();
}

void SkulMapEditScene::Start()
{
	CAM->SetLeftBottom(-WIN_WIDTH * 10, -WIN_HEIGHT * 10);
	CAM->SetRightTop(WIN_WIDTH * 10, WIN_HEIGHT * 10);
}

void SkulMapEditScene::End()
{
	CAM->SetLeftBottom(-WIN_WIDTH, -WIN_HEIGHT);
	CAM->SetRightTop(WIN_WIDTH * 2, WIN_HEIGHT * 2);

	zoom = 1.0f;
	CAM->Scale() = Vector2(1.0f, 1.0f);
	CAM->SetDeviceSize(WIN_WIDTH, WIN_HEIGHT);
}

void SkulMapEditScene::Reset()
{
}

void SkulMapEditScene::SampleTileControls()
{
	if (selectedSample == nullptr)
		return;

	if (KEY_PRESS(VK_LBUTTON) && !ImGui::GetIO().WantCaptureMouse)
	{
		if (selectedSample->GetTexture())
			editTileMap->PlaceTile(selectedSample, selectType);
	}

	if (KEY_PRESS(VK_RBUTTON) && !ImGui::GetIO().WantCaptureMouse)
	{
		editTileMap->EraseTile(selectType);
	}

	if (selectedSample->GetTexture())
	{
		if (KEY_DOWN('R'))
		{
			selectedSample->Rot().z += XM_PIDIV2;
			if (selectedSample->Rot().z > XM_2PI)
				selectedSample->Rot().z = XM_2PI;
		}
		else if (KEY_DOWN('E'))
		{
			selectedSample->Rot().z -= XM_PIDIV2;
			if (selectedSample->Rot().z < 0)
				selectedSample->Rot().z = 0;
		}

		selectedSample->Pos() = CAM->ScreenToWorld(mousePos);
		selectedSample->UpdateWorld();
	}
}

void SkulMapEditScene::ZoomControl()
{
	if (mouseWheel == 0 || !KEY_PRESS(VK_CONTROL) || ImGui::GetIO().WantCaptureMouse)
		return;

	if (mouseWheel > 0)
		zoom -= ZOOM_STEP;
	else
		zoom += ZOOM_STEP;

	zoom = Clamp(ZOOM_MIN, ZOOM_MAX, zoom);

	CAM->Scale() = Vector2(zoom, zoom);
	CAM->SetDeviceSize(WIN_WIDTH * zoom, WIN_HEIGHT * zoom);

	grid->CreateGrid(editTileMap->GetRenderSize());
}

void SkulMapEditScene::OpenSamples()
{
	ImGui::Begin("Sample Tiles", &sampleWindowOpen);

	ImGui::Text("Path: %s", ToString(curSampleFolder).c_str());

	if (ImGui::Button("Up"))
	{
		PopFolder();
		ImGui::End();
		return;
	}
	ImGui::SameLine();

	for (const wstring& folder : folders)
	{
		if (ImGui::Button(ToString(folder).c_str()))
		{
			EnterFolder(folder);
			ImGui::End();
			return;
		}
		ImGui::SameLine();
	}

	if(selectedSample->GetTexture())
		ImGui::Text("Selected: %s", ToString(selectedSample->GetTexture()->GetFile().substr(selectedSample->GetTexture()->GetFile().find_last_of(L"/") + 1)).c_str());
	else
		ImGui::Text("Selected: None");

	ImGui::NewLine();
	ImGui::Separator();
	ImGui::BeginChild("Tiles");

	ImGuiStyle& style = ImGui::GetStyle();
	float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

	for (UINT i = 0; i < sampleTiles.size(); ++i)
	{
		Texture* tex = sampleTiles[i];
		ImGui::PushID((int)i);

		if (ImGui::ImageButton("##tile", (ImTextureID)tex->GetSRV(), ImVec2(SAMPLE_SIZE, SAMPLE_SIZE)))
			ClickSampleTile(tex);

		float lastButtonX2 = ImGui::GetItemRectMax().x;
		float nextButtonX2 = lastButtonX2 + style.ItemSpacing.x + SAMPLE_SIZE;
		if (i + 1 < sampleTiles.size() && nextButtonX2 < windowVisibleX2)
			ImGui::SameLine();

		ImGui::PopID();
	}

	ImGui::EndChild();
	ImGui::End();
}

void SkulMapEditScene::ResetSampleFolder()
{
	sampleTiles.clear();
	folders.clear();

	WIN32_FIND_DATA findData;
	HANDLE handle = FindFirstFile((curSampleFolder + L"/*").c_str(), &findData);
	if (handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			wstring fileName = findData.cFileName;

			if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
				fileName != L"." && fileName != L"..")
			{
				folders.push_back(fileName);
			}
		} while (FindNextFile(handle, &findData));

		FindClose(handle);
	}

	handle = FindFirstFile((curSampleFolder + L"/*.png").c_str(), &findData);
	if (handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			wstring fileName = curSampleFolder + L"/" + findData.cFileName;
			sampleTiles.push_back(Texture::Add(fileName));
		} while (FindNextFile(handle, &findData));

		FindClose(handle);
	}
}

void SkulMapEditScene::EnterFolder(const wstring& path)
{
	curSampleFolder += L"/" + path;
	ResetSampleFolder();
}

void SkulMapEditScene::PopFolder()
{
	wstring path = curSampleFolder;
	int pos = path.find_last_of(L"/");

	if (pos != wstring::npos)
	{
		curSampleFolder = path.substr(0, pos);
		ResetSampleFolder();
	}
}

void SkulMapEditScene::ClickSampleTile(Texture* tileTex)
{
	selectedSample->SetTexture(tileTex);
}

void SkulMapEditScene::Save()
{
	if (ImGui::Button("Save"))
	{		DIALOG->OpenDialog("Save", "Save File", ".map", ".");
	}
	if (DIALOG->Display("Save"))
	{
		if (DIALOG->IsOk())
		{
			string file = DIALOG->GetFilePathName();
			file = file.substr(projectPath.length() + 1);
			editTileMap->Save(file);
		}
		DIALOG->Close();
	}
}

void SkulMapEditScene::Load()
{
	if (ImGui::Button("Load"))
		DIALOG->OpenDialog("Load", "Load File", ".map", ".");
	if (DIALOG->Display("Load"))
	{
		if (DIALOG->IsOk())
		{
			string file = DIALOG->GetFilePathName();
			file = file.substr(projectPath.length() + 1);
			editTileMap->Load(file);
		}
		DIALOG->Close();
	}
}
