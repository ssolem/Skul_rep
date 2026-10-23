#pragma once
#include "ProjectObjects/StageData.h"

class Stage;

class SpawnEditScene : public Scene
{
private:
	enum class Mode { Player, Enemy };

	const Vector2 MARKER_SIZE = { 30.0f, 30.0f };
public:
	SpawnEditScene();
	~SpawnEditScene();

	virtual void Update() override;
	virtual void Render() override;
	virtual void PostRender() override;

	virtual void Start() override;
	virtual void End() override;
	virtual void Reset() override;

private:

	void LoadStageFile(string file);
	void SaveStageFile(string file);
	void LoadMap(string mapFile);
	string ToProjectPath(string fullPath);

	void CheckClick();

	void RenderMarker(Vector2 groundPos, Float4 color);
	void RenderLabels();

	void FileUI();
	void BrushUI();
	void WaveUI();
	void SpawnUI();
	bool TypeCombo(const char* label, string& type);

private:
	Stage* stage = nullptr;
	StageData data;
	string stageFile;
	string projectPath;

	Mode mode = Mode::Enemy;
	int curWave = 0;
	int spawnNum = -1;
	string brushType = "Ent";

	RectCollider* marker;
	RectCollider* passLine;
	float zoom = 1.0f;

	string status;
};

