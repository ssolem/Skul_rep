#pragma once
class TileMapEditScene : public Scene
{
private:
	const float SAMPLE_SIZE = 50.0f;	//샘플 타일의 크기

public:
	TileMapEditScene();
	~TileMapEditScene();

	virtual void Update() override;
	virtual void Render() override;
	virtual void PostRender() override;

private:
	//편집용 기능들
	void ClickSampleTile(void* sampleTile);	//샘플 타일을 클릭했을 때
	void CreateSample();					//샘플 메뉴 만들기

	//세이브와 로드 호출
	void Save();
	void Load();

private:
	EditTileMap* editTileMap;

	//편집용 기능들
	vector<Button*> sampleTiles;	//편집메뉴로서의 선택지들

	Quad* selectedSample;			//그래서 지금 무슨 타일을 선택했는가
	Tile::Type selectType;			//선택한 타일의 종류	

	string projectPath;				//현재 이 프로젝트가 있는 폴더
};

