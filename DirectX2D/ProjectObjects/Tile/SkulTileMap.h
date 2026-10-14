#pragma once
class SkulTileMap : public Transform
{
public:
	SkulTileMap(string file, Vector2 pos = { 0, 0 });
	~SkulTileMap();

	void Render();
	Tile* CheckFloorTileCollision(Collider* collider, Vector2* overlap = nullptr);
	Tile* CheckStairTileCollision(Collider* collider, Vector2* overlap = nullptr);
	Tile* CheckWallTileCollision(Collider* collider, Vector2* overlap = nullptr);

	UINT GetWidth() { return width; }
	//맵의 월드 범위 - 파일엔 칸 수(width/height)만 있고 시작 위치가 없어서, 로드할 때 실제 타일 인덱스로 계산
	Vector2 GetLeftBottom() { return GlobalPos() + minIndex * tileSize * GlobalScale(); }
	Vector2 GetRightTop() { return GlobalPos() + (maxIndex + Vector2(1.0f, 1.0f)) * tileSize * GlobalScale(); }
	Vector2 GetMidPos() { return (GetLeftBottom() + GetRightTop()) * 0.5f; }
	Vector2 GetMinIndex() { return minIndex; }

	map<Vector2, Tile*, Vector2IntLess>& GetFloorTiles() { return floorTiles; }
	map<Vector2, Tile*, Vector2IntLess>& GetStairTiles() { return stairTiles; }
	map<Vector2, Tile*, Vector2IntLess>& GetWallTiles() { return wallTiles; }
	Vector2 WorldToGridPos(Vector2 worldPos);

private:
	void Load(string file); //지형 로딩용
	void LoadFloorTiles(BinaryReader* reader);
	void LoadStairTiles(BinaryReader* reader);
	void LoadWallTiles(BinaryReader* reader);
	void ExpandBounds(Vector2 index);	//로드한 타일 인덱스로 맵 범위 갱신

private:
	UINT width, height;
	Vector2 minIndex = Vector2(FLT_MAX, FLT_MAX);	//실제 타일이 있는 최소/최대 인덱스 (에디터에서 칠한 위치 그대로라 0부터 시작하지 않음)
	Vector2 maxIndex = Vector2(-FLT_MAX, -FLT_MAX);
	Vector2 tileSize; //지형 단위 크기
	Vector2 tileScale = { 2.0f, 2.0f };
	map<Vector2, Quad*, Vector2IntLess> bgTiles;		
	map<Vector2, Tile*, Vector2IntLess> stairTiles;	
	map<Vector2, Tile*, Vector2IntLess> floorTiles;	
	map<Vector2, Tile*, Vector2IntLess> wallTiles;
};

