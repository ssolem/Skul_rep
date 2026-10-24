#include "Framework.h"
#include "StageData.h"
#include <fstream>
#include "Libraries/nlohmann/json.hpp"

using json = nlohmann::ordered_json;

namespace
{
	json ToJson(Vector2 v)
	{
		return json::array({ v.x, v.y });
	}

	Vector2 ToVector2(const json& j)
	{
		return Vector2(j.at(0).get<float>(), j.at(1).get<float>());
	}

}

bool StageDataIO::Load(string file, StageData& out)
{
	ifstream in(file);
	if (!in.is_open())
	{
		return false;
	}

	//형식이 틀리면 json이 예외를 던짐 - 게임이 죽지 않게 여기서 받음
	try
	{
		json j = json::parse(in);
		StageData data;

		data.mapFile = j.at("map").get<string>();
		data.playerSpawn = ToVector2(j.at("playerSpawn"));

		for (const json& jw : j.at("waves"))
		{
			WaveData wave;

			string startText = jw.value("start", "PrevCleared");
			StringToWaveStart(startText, wave.start);

			wave.value = jw.value("value", 0.0f);

			for (const json& js : jw.at("spawns"))
			{
				SpawnData spawn;
				spawn.type = js.at("type").get<string>();
				spawn.pos = ToVector2(js.at("pos"));
				spawn.delay = js.value("delay", 0.0f);

				wave.spawns.push_back(spawn);
			}

			data.waves.push_back(wave);
		}

		out = data;
		return true;
	}
	catch (const json::exception& e)
	{
		return false;
	}
}

bool StageDataIO::Save(string file, const StageData& data)
{
	json j;
	j["map"] = data.mapFile;
	j["playerSpawn"] = ToJson(data.playerSpawn);
	j["waves"] = json::array();

	for (const WaveData& wave : data.waves)
	{
		json jw;
		jw["start"] = WaveStartToString(wave.start);
		jw["value"] = wave.value;
		jw["spawns"] = json::array();

		for (const SpawnData& spawn : wave.spawns)
		{
			json js;
			js["type"] = spawn.type;
			js["pos"] = ToJson(spawn.pos);
			js["delay"] = spawn.delay;

			jw["spawns"].push_back(js);
		}

		j["waves"].push_back(jw);
	}

	ofstream outFile(file);
	if (!outFile.is_open())
	{
		return false;
	}

	outFile << j.dump(2);	//들여쓰기 2칸
	return true;
}

string StageDataIO::WaveStartToString(WaveStart start)
{
	switch (start)
	{
	case WaveStart::PrevCleared: return "PrevCleared";
	case WaveStart::PlayerPassX: return "PlayerPassX";
	}

	return "PrevCleared";
}

bool StageDataIO::StringToWaveStart(string text, WaveStart& out)
{
	if (text == "PrevCleared") { out = WaveStart::PrevCleared; return true; }
	if (text == "PlayerPassX") { out = WaveStart::PlayerPassX; return true; }

	out = WaveStart::PrevCleared;
	return false;
}
