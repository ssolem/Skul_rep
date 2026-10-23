#include "Framework.h"
#include "SpawnEditScene.h"
#include "ProjectObjects/Stage.h"
#include "ProjectObjects/StageData.h"
#include "ProjectObjects/Character/Enemy/EnemyFactory.h"

SpawnEditScene::SpawnEditScene()
{
	char path[256];
	GetCurrentDirectoryA(sizeof(path), path);
	projectPath = path;

	marker = new RectCollider(Vector2(30.0f, 30.0f));

	LoadStageFile("ProjectMaps/Chapter1_02.stage.json");
}

SpawnEditScene::~SpawnEditScene()
{
	delete stage;
	delete marker;
}

void SpawnEditScene::Update()
{
	CheckClick();
}

void SpawnEditScene::Render()
{
	if (stage)
		stage->Render();
}

void SpawnEditScene::PostRender()
{
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
		return;

	data = load;
	stageFile = file;

	if (data.waves.empty())
		data.waves.push_back(WaveData());

	curWave = 0;
	spawnNum = -1;

	LoadMap(data.mapFile);
}

void SpawnEditScene::SaveStageFile(string file)
{
	//저장 실패시
	if (!StageDataIO::Save(file, data))
		return;

	stageFile = file;
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
