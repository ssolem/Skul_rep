#pragma once

class Stage;
class Enemy;
struct StageData;

//언제 Spawn할지 관리
class WaveSpawner
{
private:
	struct SpawnInfo
	{
		Enemy* enemy;	
		Vector2 pos;	
		float delay;	
		bool spawned;	
	};

	struct WaveInfo
	{
		WaveStart start;
		float value;
		vector<SpawnInfo> spawns;
	};

public:
	WaveSpawner(Stage* stage) : stage(stage) {}

	void Build(const StageData& data);	
	void Start();						
	void Update();

	void SetOnClear(function<void()> callback) { onClear = callback; }
	int GetCurWave() { return curWave; }
	int GetWaveCount() { return (int)waves.size(); }
	bool IsCleared() { return isCleared; }

private:
	void StartWave(int index);
	void SpawnDueEnemies();		
	bool IsAllSpawned();		
	bool CanStartNextWave();	
	bool IsStageCleared();

private:
	Stage* stage;
	vector<WaveInfo> waves;

	int curWave = -1;		//-1: 아직 시작 안 함
	float waveTime = 0.0f;	
	bool isCleared = false;

	function<void()> onClear;
};
