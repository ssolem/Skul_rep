#pragma once

class EditTileMap : public Transform // 편집 중에 '판'으로서 기능하도록 하려고
{
public:
	EditTileMap(UINT width, UINT height);
	~EditTileMap();

	void Update();
	void Render();

	void ClickTile(Quad* selectSample, Tile::Type type);  //클릭(지정)한 타일에 작용
	void DeleteTile(Tile::Type type); //지정된 타일을 지우기

	void Save(string file);
	void Load(string file);

private:
	void CreateTile();

	void SetBGTile(wstring file, float angle);
	void SetObjTile(wstring file, float angle);
	void SetBushTile(wstring file, float angle);

private:
	UINT width, height; //지도의 크기
	Vector2 tileSize; //타일의 크기

	vector<Tile*> bgTiles;		// 배경 타일
	vector<Tile*> objTiles;		// 장애물 타일
	vector<Tile*> bushTiles;	// 험지 타일
};

