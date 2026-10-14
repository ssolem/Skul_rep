#include "Framework.h"
#include "GameTileMap.h"

GameTileMap::GameTileMap(string file, Vector2 pos)
{
	Pos() = pos;
	UpdateWorld();	//맵 위치 지정

	Load(file);
}

GameTileMap::~GameTileMap()
{
	for (Tile* tile : objTiles) delete tile;
	for (Tile* tile : bushTiles) delete tile;
	for (Quad* tile : bgTiles) delete tile;
}

void  GameTileMap::Render()
{
	for (Quad* tile : bgTiles) tile->Render();
	for (Tile* tile : bushTiles) tile->Render();
	for (Tile* tile : objTiles) tile->Render();
}

void GameTileMap::GetNodes(vector<Node*>& nodes)
{
	//노드 정보 따기

	for (Quad* tile : bgTiles)
	{
		Vector2 tilePos = tile->GlobalPos();
		Node* node = new Node(tilePos, nodes.size());

		for (Tile* obj : objTiles)
		{
			Vector2 objPos = obj->GlobalPos();

			if (objPos == tilePos)
				node->SetState(Node::State::OBSTACLE);
		}

		nodes.push_back(node);
		//for (Tile* bush : bushTiles) {} 덤불에 할 일이 있다면
	}
}

void GameTileMap::PushObject(Collider* collider)
{
	for (Tile* tile : objTiles)
		tile->GetCollider()->PushCollider(collider);
}

void GameTileMap::Load(string file)
{
	BinaryReader* reader = new BinaryReader(file);

	width = reader->UInt();
	height = reader->UInt();

	UINT size = reader->UInt();
	bgTiles.resize(size);	//여기까진 같다

	// 배경이 편집 대상이 아니라 출력 전용 쿼드

	for (Quad*& tile : bgTiles)
	{
		Tile::Data data;

		data.textureFile = reader->WString();
		data.pos.x = reader->Float();
		data.pos.y = reader->Float();
		data.angle = reader->Float();
		data.type = (Tile::Type)reader->Int(); //타일 전체를 읽는게 아니어도 데이터는 일단
												// 데이터 로드 순서가 꼬이지 않도록

		tile = new Quad(data.textureFile);		//배경은 그림만 출력
		tile->Pos() = data.pos;
		tile->Rot().z = data.angle;
		tile->SetParent(this);
		tile->UpdateWorld();
	}

	for (Tile* tile : objTiles) delete tile; 

	size = reader->UInt();
	objTiles.resize(size);

	for (Tile*& tile : objTiles)
	{
		Tile::Data data; 

		data.textureFile = reader->WString();
		data.pos.x = reader->Float();
		data.pos.y = reader->Float();
		data.angle = reader->Float();
		data.type = (Tile::Type)reader->UInt();

		tile = new Tile(data);
		tile->SetParent(this);
		tile->Update();
	}

	size = reader->UInt();
	bushTiles.resize(size);

	for (Tile*& tile : bushTiles)
	{
		Tile::Data data;

		data.textureFile = reader->WString();
		data.pos.x = reader->Float();
		data.pos.y = reader->Float();
		data.angle = reader->Float();
		data.type = (Tile::Type)reader->UInt();

		tile = new Tile(data);
		tile->SetParent(this);
		tile->Update();
	}

	delete reader;
}
