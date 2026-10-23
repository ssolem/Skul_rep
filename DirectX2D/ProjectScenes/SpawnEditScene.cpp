#include "Framework.h"
#include "SpawnEditScene.h"
#include "ProjectObjects/Stage.h"
#include "ProjectObjects/StageData.h"
#include "ProjectObjects/Character/Enemy/EnemyFactory.h"
#include <algorithm>

SpawnEditScene::SpawnEditScene()
{
	char path[256];
	GetCurrentDirectoryA(sizeof(path), path);
	projectPath = path;

	marker = new RectCollider(MARKER_SIZE);

	//PlayerPassX 웨이브의 x좌표 표시
	passLine = new RectCollider(Vector2(2.0f, WIN_HEIGHT));
	passLine->GetColor() = { 1.0f, 0.5f, 0.0f, 1.0f };

	LoadStageFile("ProjectMaps/Chapter1_02.stage.json");

	if (data.waves.empty())
		data.waves.push_back(WaveData());
}

SpawnEditScene::~SpawnEditScene()
{
	delete stage;
	delete marker;
	delete passLine;
}

void SpawnEditScene::Update()
{
	CheckClick();

	if (KEY_DOWN(VK_F1))
	{
		SceneManager::Get()->Add("TestCharacter Scene");
		SceneManager::Get()->Remove("SpawnEdit Scene");
	}
}

void SpawnEditScene::Render()
{
	if (stage)
		stage->Render();

	for (int w = 0; w < (int)data.waves.size(); w++)
	{
		for (int i = 0; i < (int)data.waves[w].spawns.size(); i++)
		{
			const SpawnData& spawn = data.waves[w].spawns[i];

			Float4 color;
			if (!EnemyFactory::IsValidType(spawn.type))	color = { 1.0f, 0.0f, 0.0f, 1.0f };	//모르는 이름: 빨강
			else if (w != curWave)						color = { 1.0f, 1.0f, 1.0f, 0.3f };	//다른 웨이브: 흐리게
			else if (i == spawnNum)						color = { 1.0f, 1.0f, 0.0f, 1.0f };	//선택: 노랑
			else										color = { 0.0f, 1.0f, 0.0f, 1.0f };	//현재 웨이브: 초록

			RenderMarker(spawn.pos, color);
		}
	}

	//현재 웨이브가 x좌표 조건이면 그 위치에 세로선 - 카메라를 따라 화면 높이에 맞춤
	if (!data.waves.empty() && data.waves[curWave].start == WaveStart::PlayerPassX)
	{
		passLine->Pos() = { data.waves[curWave].value, CAM->Pos().y + WIN_HEIGHT * 0.5f };
		passLine->UpdateWorld();
		passLine->Render();
	}

	RenderMarker(data.playerSpawn, { 0.0f, 0.6f, 1.0f, 1.0f });	//플레이어: 파랑
}

void SpawnEditScene::PostRender()
{
	RenderLabels();

	ImGui::Begin("Spawn Editor");
	FileUI();
	ImGui::Separator();
	BrushUI();
	ImGui::Separator();
	WaveUI();
	ImGui::Separator();
	SpawnUI();
	ImGui::End();
}

void SpawnEditScene::Start()
{
	CAM->SetTarget(nullptr);
	if (stage) stage->SetCameraBounds();
}

void SpawnEditScene::End()
{
}

void SpawnEditScene::Reset()
{
}

void SpawnEditScene::LoadStageFile(string file)
{
	StageData load;
	if (!StageDataIO::Load(file, load))
	{
		status = "Load failed: " + file;
		return;
	}

	data = load;
	stageFile = file;

	if (data.waves.empty())
		data.waves.push_back(WaveData());

	curWave = 0;
	spawnNum = -1;

	LoadMap(data.mapFile);

	status = "Loaded: " + file;
}

void SpawnEditScene::SaveStageFile(string file)
{
	//저장 실패시
	if (!StageDataIO::Save(file, data))
	{
		status = "Save failed: " + file;
		return;
	}

	stageFile = file;
	status = "Saved: " + file;
}

void SpawnEditScene::LoadMap(string mapFile)
{
	delete stage;

	stage = new Stage(mapFile);
	stage->SetCameraBounds();

	if (data.mapFile != mapFile)
	{
		data.mapFile = mapFile;
	}
}

string SpawnEditScene::ToProjectPath(string fullPath)
{
	string path = fullPath.substr(projectPath.length() + 1);
	replace(path.begin(), path.end(), '\\', '/');
	return path;
}

void SpawnEditScene::CheckClick()
{
	if (!stage) return;
	if (ImGui::GetIO().WantCaptureMouse) return;
	if (!KEY_DOWN(VK_LBUTTON)) return;

	Vector2 mouseWorld = CAM->ScreenToWorld(mousePos);
	Vector2 groundPos = stage->GetGroundPos(mouseWorld);

	if (groundPos.y == mouseWorld.y)
		return;

	if (mode == Mode::Player)
		data.playerSpawn = groundPos;
	else
	{
		SpawnData spawn;
		spawn.type = brushType;
		spawn.pos = groundPos;
		data.waves[curWave].spawns.push_back(spawn);
		spawnNum = (int)data.waves[curWave].spawns.size() - 1;
	}
}

void SpawnEditScene::RenderMarker(Vector2 groundPos, Float4 color)
{
	marker->Pos() = groundPos + Vector2(0.0f, MARKER_SIZE.y * 0.5f);
	marker->GetColor() = color;
	marker->UpdateWorld();
	marker->Render();
}

//마커 위 이름 - 화면 좌표로 바꿔서 그림 (현재 웨이브만)
void SpawnEditScene::RenderLabels()
{
	if (data.waves.empty()) return;

	Vector2 labelOffset(0.0f, MARKER_SIZE.y + 15.0f);

	for (const SpawnData& spawn : data.waves[curWave].spawns)
	{
		Vector2 screenPos = CAM->WorldToScreen(spawn.pos + labelOffset);
		Font::Get()->RenderText(spawn.type, { screenPos.x, screenPos.y });
	}

	Vector2 playerScreen = CAM->WorldToScreen(data.playerSpawn + labelOffset);
	Font::Get()->RenderText("Player", { playerScreen.x, playerScreen.y });
}

//파일 - 스테이지 json 읽기/저장, 맵 교체
void SpawnEditScene::FileUI()
{
	ImGui::Text("Stage: %s", stageFile.empty() ? "(new)" : stageFile.c_str());
	ImGui::Text("Map: %s", data.mapFile.c_str());

	if (ImGui::Button("Load"))
		DIALOG->OpenDialog("LoadStage", "Load Stage", ".json", ".");
	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		if (stageFile.empty())
			DIALOG->OpenDialog("SaveStage", "Save Stage", ".json", ".");
		else
			SaveStageFile(stageFile);
	}
	ImGui::SameLine();
	if (ImGui::Button("Save As"))
		DIALOG->OpenDialog("SaveStage", "Save Stage", ".json", ".");
	ImGui::SameLine();
	if (ImGui::Button("Change Map"))
		DIALOG->OpenDialog("ChangeMap", "Change Map", ".map", ".");

	if (DIALOG->Display("LoadStage"))
	{
		if (DIALOG->IsOk())
			LoadStageFile(ToProjectPath(DIALOG->GetFilePathName()));
		DIALOG->Close();
	}
	if (DIALOG->Display("SaveStage"))
	{
		if (DIALOG->IsOk())
			SaveStageFile(ToProjectPath(DIALOG->GetFilePathName()));
		DIALOG->Close();
	}
	if (DIALOG->Display("ChangeMap"))
	{
		if (DIALOG->IsOk())
			LoadMap(ToProjectPath(DIALOG->GetFilePathName()));
		DIALOG->Close();
	}

	if (!status.empty())
		ImGui::Text("%s", status.c_str());
}


void SpawnEditScene::BrushUI()
{
	int modeIndex = (int)mode;
	ImGui::RadioButton("Player", &modeIndex, (int)Mode::Player);
	ImGui::SameLine();
	ImGui::RadioButton("Enemy", &modeIndex, (int)Mode::Enemy);
	mode = (Mode)modeIndex;

	if (mode == Mode::Enemy)
		TypeCombo("Brush", brushType);
}

//웨이브 추가/삭제/선택, 시작 조건
void SpawnEditScene::WaveUI()
{
	if (ImGui::Button("+ Wave"))
	{
		data.waves.push_back(WaveData());
		curWave = (int)data.waves.size() - 1;
		spawnNum = -1;
	}
	ImGui::SameLine();
	if (ImGui::Button("- Wave") && data.waves.size() > 1)	//마지막 하나는 남김
	{
		data.waves.erase(data.waves.begin() + curWave);
		if (curWave >= (int)data.waves.size())
			curWave = (int)data.waves.size() - 1;
		spawnNum = -1;
	}

	for (int w = 0; w < (int)data.waves.size(); w++)
	{
		string label = "Wave " + to_string(w) + " (" + to_string(data.waves[w].spawns.size()) + ")";
		if (ImGui::RadioButton(label.c_str(), curWave == w))
		{
			curWave = w;
			spawnNum = -1;
		}
	}

	WaveData& wave = data.waves[curWave];
	if (ImGui::BeginCombo("Start", StageDataIO::WaveStartToString(wave.start).c_str()))
	{
		for (WaveStart start : { WaveStart::PrevCleared, WaveStart::PlayerPassX })
		{
			if (ImGui::Selectable(StageDataIO::WaveStartToString(start).c_str(), wave.start == start))
			{
				//x좌표 조건으로 바꾸면 처음 값은 지금 화면 가운데
				if (start == WaveStart::PlayerPassX && wave.start != start)
					wave.value = CAM->Pos().x + WIN_WIDTH * 0.5f;

				wave.start = start;
			}
		}
		ImGui::EndCombo();
	}

	if (wave.start == WaveStart::PlayerPassX)
		ImGui::DragFloat("Pass X", &wave.value, 1.0f, 0.0f, 0.0f, "%.0f");
}

//현재 웨이브의 스폰 목록
void SpawnEditScene::SpawnUI()
{
	vector<SpawnData>& spawns = data.waves[curWave].spawns;

	ImGui::BeginChild("SpawnList", ImVec2(0, 120), true);
	for (int i = 0; i < (int)spawns.size(); i++)
	{
		string label = to_string(i) + ": " + spawns[i].type;
		if (ImGui::Selectable(label.c_str(), spawnNum == i))
			spawnNum = i;
	}
	ImGui::EndChild();

	if (spawnNum < 0 || spawnNum >= (int)spawns.size())
		return;

	SpawnData& spawn = spawns[spawnNum];
	TypeCombo("Type", spawn.type);
	ImGui::DragFloat("Delay", &spawn.delay, 0.05f, 0.0f, 30.0f, "%.2f s");
	ImGui::Text("Pos: %.0f, %.0f", spawn.pos.x, spawn.pos.y);

	if (ImGui::Button("Delete"))
	{
		spawns.erase(spawns.begin() + spawnNum);
		spawnNum = -1;
	}
}

//팩토리에 등록된 이름 중 하나를 고르는 콤보
bool SpawnEditScene::TypeCombo(const char* label, string& type)
{
	bool changed = false;

	if (ImGui::BeginCombo(label, type.c_str()))
	{
		for (const string& name : EnemyFactory::GetTypeNames())
		{
			bool selected = (name == type);
			if (ImGui::Selectable(name.c_str(), selected))
			{
				type = name;
				changed = true;
			}
			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	return changed;
}
