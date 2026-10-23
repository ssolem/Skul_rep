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

	StartWave(0);
}

void WaveSpawner::StartWave(int index)
{
	curWave = index;
	waveTime = 0.0f;
}

void WaveSpawner::Update()
{
	if (curWave < 0 || isCleared) return;

	waveTime += DELTA;
	SpawnDueEnemies();

	bool hasNextWave = curWave + 1 < (int)waves.size();

	if (hasNextWave)
	{
		if (CanStartNextWave())
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
	for (SpawnInfo& info : waves[curWave].spawns)
		if (!info.spawned) return false;

	return true;
}

bool WaveSpawner::CanStartNextWave()
{
	WaveInfo& next = waves[curWave + 1];

	switch (next.start)
	{
	case WaveStart::PrevCleared:
		return IsAllSpawned() && stage->GetRemainEnemyCount() == 0;
	}

	return false;
}

bool WaveSpawner::IsStageCleared()
{
	return IsAllSpawned() && stage->GetRemainEnemyCount() == 0;
}
