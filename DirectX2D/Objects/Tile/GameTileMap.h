#pragma once

class GameTileMap : public Transform
{
public:
	GameTileMap(string file, Vector2 pos = { 0, 0 });
	~GameTileMap();

	void Render();
	void GetNodes(vector<Node*>& nodes);
	void PushObject(Collider* collider);

	UINT GetWidth() { return width; }

	vector<Tile*> GetObjTiles() { return objTiles; }

private:
	void Load(string file); //지형 로딩용

private:
	UINT width, height;
	Vector2 tileSize; //지형 단위 크기
	vector<Quad*> bgTiles;		// 배경 (이동에 기본적인 영향만)
	vector<Tile*> bushTiles;	// 험지 타일
	vector<Tile*> objTiles;		// 장애물 타일
};

