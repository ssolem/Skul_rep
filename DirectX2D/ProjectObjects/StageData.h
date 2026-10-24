#pragma once

//웨이브 안에서 누가, 어디서, 웨이브 시작 몇 초 뒤에 나오는지
struct SpawnData
{
	string type = "Ent";
	Vector2 pos;
	float delay = 0.0f;
};

//웨이브가 언제 시작하는지 그 웨이브에 나올 적들
struct WaveData
{
	WaveStart start = WaveStart::PrevCleared;
	float value = 0.0f;	
	vector<SpawnData> spawns;
};

struct StageData
{
	string mapFile;
	Vector2 playerSpawn;
	Vector2 door;			//보상 문 바닥 위치
	bool hasDoor = false;	//false면 문 없는 방 - (0,0)과 구분하려고 따로 둠
	vector<WaveData> waves;
};

class StageDataIO
{
public:
	static bool Load(string file, StageData& out);	
	static bool Save(string file, const StageData& data);

	static string WaveStartToString(WaveStart start);
	static bool StringToWaveStart(string text, WaveStart& out);
};
