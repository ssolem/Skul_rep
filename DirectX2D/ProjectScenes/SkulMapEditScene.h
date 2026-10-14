#pragma once

class SkulMapGrid;
class SkulEditTileMap;

class SkulMapEditScene : public Scene
{
public:
	SkulMapEditScene();
	~SkulMapEditScene();

	virtual void Update() override;
	virtual void Render() override;
	virtual void PostRender() override;

	virtual void Start() override;
	virtual void End() override;
	virtual void Reset() override;

private:

	void SampleTileControls();
	void ZoomControl();
	void OpenSamples();

	void ResetSampleFolder();	
	void EnterFolder(const wstring& path);
	void PopFolder();
	void ClickSampleTile(Texture* tileTex);

	void Save();
	void Load();

private:
	const float SAMPLE_SIZE = 50.0f;
	const wstring SAMPLE_FOLDER = L"ProjectTextures/Tile";

	const float ZOOM_STEP = 0.1f;
	const float ZOOM_MIN = 0.25f;
	const float ZOOM_MAX = 4.0f;
	float zoom = 1.0f;

	SkulMapGrid* grid;
	SkulEditTileMap* editTileMap;

	vector<Texture*> sampleTiles;

	Quad* selectedSample;			
	Tile::Type selectType;			

	string projectPath;
	
	bool sampleWindowOpen = true;
	wstring curSampleFolder = SAMPLE_FOLDER;
	vector<wstring> folders;
};

