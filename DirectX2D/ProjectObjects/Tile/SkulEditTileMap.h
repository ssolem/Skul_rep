#pragma once

class SkulEditTileMap : public Transform
{
public:
	SkulEditTileMap();
	~SkulEditTileMap();

	void Update();
	void Render();

	void PlaceTile(Quad* selectSample, Tile::Type type);
	void EraseTile(Tile::Type type);
	void SelectBaseTile();

	void Save(string file);
	void Load(string file);

	Vector2 GetRenderSize() { return renderSize; }

	void SetTileSize(Vector2 size) { tileSize = size; SetRenderSize(); }
	void SetTileScale(Vector2 scale);	// 세팅 시 배치된 모든 타일의 스케일에 반영된다
	void SetRenderSize() { renderSize = {tileSize.x * tileScale.x, tileSize.y * tileScale.y}; }

private:
	Vector2 GetTileIndex()
	{
		Vector2 w = CAM->ScreenToWorld(mousePos);
		return { floorf(w.x / renderSize.x), floorf(w.y / renderSize.y) };
	}

	void SetBGTile(wstring file, Vector2 index, float angle);
	void SetFloorTile(wstring file, Vector2 index, float angle);
	void SetStairTile(wstring file, Vector2 index, float angle);
	void SetWallTile(wstring file, Vector2 index, float angle);

	void CreateTile(wstring file, Vector2 index, float angle, Tile::Type type);
	void RecalculateTileMapSize();

	void SaveTiles(BinaryWriter* writer, map<Vector2, Tile*, Vector2IntLess>& tiles);
	void LoadTiles(BinaryReader* reader, map<Vector2, Tile*, Vector2IntLess>& tiles);

private:
	UINT width = 1;
	UINT height = 1; 

	Vector2 tileSize = Vector2::One;	//타일의 크기
	Vector2 tileScale = Vector2::One;	//타일의 스케일
	Vector2 renderSize = Vector2::One;	//타일의 렌더링 크기

	Vector2 leftBottom = Vector2::Zero;	//좌하단 좌표
	Vector2 rightTop = Vector2::Zero;	//우상단 좌표

	map<Vector2, Tile*, Vector2IntLess> bgTiles;	   // 배경 타일
	map<Vector2, Tile*, Vector2IntLess> floorTiles;	   // 바닥 타일
	map<Vector2, Tile*, Vector2IntLess> stairTiles;	   // 발판 타일
	map<Vector2, Tile*, Vector2IntLess> wallTiles;	   // 벽 타일
};