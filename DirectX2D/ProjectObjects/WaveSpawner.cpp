#include "Framework.h"
#include "WaveSpawner.h"
#include "Stage.h"
#include "StageData.h"
#include "Character/Enemy/EnemyFactory.h"
#include "Character/Enemy/Normal/Enemy.h"

//struct WaveData
//{
//	WaveStart start = WaveStart::PrevCleared;
//	float value = 0.0f;
//	vector<SpawnData> spawns;
//};

//struct SpawnData
//{
//	string type = "Ent";
//	Vector2 pos;
//	float delay = 0.0f;
//};

void WaveSpawner::Build(const StageData& data)
{
	for (const WaveData& waveData : data.waves)
	{
		WaveInfo wave = { waveData.start, waveData.value, {} };

		for (const SpawnData& spawnData : waveData.spawns)
		{
			Enemy* enemy = EnemyFactory::Create(spawnData.type);
			if (enemy == nullptr) continue;

			stage->RegisterEnemy(enemy);

			Vector2 footPos = stage->GetGroundPos(spawnData.pos);
			wave.spawns.push_back({ enemy, footPos, spawnData.delay, false });
		}

		waves.push_back(wave);
	}
}

void WaveSpawner::Start()
{
	if (waves.empty()) return;

	isRunning = true;	//첫 웨이브도 Update에서 조건을 보고 시작
}

void WaveSpawner::StartWave(int index)
{
	curWave = index;
	waveTime = 0.0f;
}

void WaveSpawner::Update()
{
	if (!isRunning || isCleared) return;

	if (curWave >= 0)
	{
		waveTime += DELTA;
		SpawnDueEnemies();
	}

	bool hasNextWave = curWave + 1 < (int)waves.size();

	if (hasNextWave)
	{
		if (CanStartWave(curWave + 1))
			StartWave(curWave + 1);
	}
	else if (IsStageCleared())
	{
		isCleared = true;
		if (onClear) onClear();
	}
}

void WaveSpawner::SpawnDueEnemies()
{
	for (SpawnInfo& info : waves[curWave].spawns)
	{
		if (info.spawned) continue;
		if (waveTime < info.delay) continue;

		info.enemy->Spawn(info.pos);
		info.spawned = true;
	}
}

bool WaveSpawner::IsAllSpawned()
{
	if (curWave < 0) return true;	//첫 웨이브 전 - 나올 적 없음

	for (SpawnInfo& info : waves[curWave].spawns)
		if (!info.spawned) return false;

	return true;
}

//이전 웨이브가 다 나오기 전에는 시작하지 않음 - 안 그러면 남은 적이 영영 안 나옴
bool WaveSpawner::CanStartWave(int index)
{
	if (!IsAllSpawned()) return false;

	WaveInfo& wave = waves[index];

	switch (wave.start)
	{
	case WaveStart::PrevCleared:
		return stage->GetRemainEnemyCount() == 0;

	case WaveStart::PlayerPassX:
	{
		Character* player = stage->GetPlayer();
		return player && player->GlobalPos().x >= wave.value;	//오른쪽으로 진행하는 맵 기준
	}
	}

	return false;
}

bool WaveSpawner::IsStageCleared()
{
	return IsAllSpawned() && stage->GetRemainEnemyCount() == 0;
}
