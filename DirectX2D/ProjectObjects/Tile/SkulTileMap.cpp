#include "Framework.h"
#include "SkulTileMap.h"

SkulTileMap::SkulTileMap(string file, Vector2 pos)
{
	Pos() = pos;
	UpdateWorld();	//¸Ê À§Ä¡ ÁöÁ¤

	Load(file);
}

SkulTileMap::~SkulTileMap()
{
	for (auto& pair : bgTiles) delete pair.second;
	for (auto& pair : floorTiles) delete pair.second;
	for (auto& pair : stairTiles) delete pair.second;
	for (auto& pair : wallTiles) delete pair.second;
}

void  SkulTileMap::Render()
{
	for (auto& pair : bgTiles) pair.second->Render();
	for (auto& pair : floorTiles) pair.second->Render();
	for (auto& pair : stairTiles) pair.second->Render();
	for (auto& pair : wallTiles) pair.second->Render();
}

Tile* SkulTileMap::CheckFloorTileCollision(Collider* collider, Vector2* overlap)
{
	for (auto& pair : floorTiles)
		if (pair.second->GetCollider()->IsCollision(collider, overlap))
			return pair.second;

	return nullptr;
}

Tile* SkulTileMap::CheckStairTileCollision(Collider* collider, Vector2* overlap)
{
	for (auto& pair : stairTiles)
		if (pair.second->GetCollider()->IsCollision(collider, overlap))
			return pair.second;

	return nullptr;
}

Tile* SkulTileMap::CheckWallTileCollision(Collider* collider, Vector2* overlap)
{
	for (auto& pair : wallTiles)
		if (pair.second->GetCollider()->IsCollision(collider, overlap))
			return pair.second;

	return nullptr;
}

void SkulTileMap::Load(string file)
{
	BinaryReader* reader = new BinaryReader(file);

	minIndex = Vector2(FLT_MAX, FLT_MAX);
	maxIndex = Vector2(-FLT_MAX, -FLT_MAX);

	width = reader->UInt();
	height = reader->UInt();
	tileSize.x = (int)reader->Float();
	tileSize.y = (int)reader->Float();

	Scale() = tileScale;
	UpdateWorld();

	for (auto& pair : bgTiles) delete pair.second;
	bgTiles.clear();

	UINT size = reader->UInt();

	for (UINT i = 0; i < size; ++i)
	{
		Tile::Data data = {};

		data.textureFile = reader->WString();
		data.pos.x = reader->Float();
		data.pos.y = reader->Float();
		data.angle = reader->Float();
		data.type = (Tile::Type)reader->UInt();
		Vector2 index = { floorf(data.pos.x / tileSize.x),	floorf(data.pos.y / tileSize.y) };
		ExpandBounds(index);

		Quad* tile = new Quad(data.textureFile);
		tile->Pos() = data.pos;
		tile->Rot().z = data.angle;
		tile->SetParent(this);
		tile->UpdateWorld();
		bgTiles[index] = tile;
	}

	LoadFloorTiles(reader);
	LoadStairTiles(reader);
	LoadWallTiles(reader);

	delete reader;
}

void SkulTileMap::LoadFloorTiles(BinaryReader* reader)
{
	for (auto& pair : floorTiles) delete pair.second;
	floorTiles.clear();

	UINT size = reader->UInt();

	for (UINT i = 0; i < size; ++i)
	{
		Tile::Data data = {};

		data.textureFile = reader->WString();
		data.pos.x = reader->Float();
		data.pos.y = reader->Float();
		data.angle = reader->Float();
		data.type = (Tile::Type)reader->UInt();
		Vector2 index = { floorf(data.pos.x / tileSize.x),	floorf(data.pos.y / tileSize.y) };
		ExpandBounds(index);
		Tile* tile = new Tile(data);
		tile->SetTag("ObjectTile");
		tile->SetParent(this);

		RectCollider* col = tile->GetCollider();
		col->Scale() = { 1.0f, 0.85f };
		col->Pos().y -= (1.0f -col->Scale().y) * tile->GetSize().y;
		col->UpdateWorld();

		tile->Update();
		floorTiles[index] = tile;
	}
}

void SkulTileMap::LoadStairTiles(BinaryReader* reader)
{
	for (auto& pair : stairTiles) delete pair.second;
	stairTiles.clear();

	UINT size = reader->UInt();

	for (UINT i = 0; i < size; ++i)
	{
		Tile::Data data = {};

		data.textureFile = reader->WString();
		data.pos.x = reader->Float();
		data.pos.y = reader->Float();
		data.angle = reader->Float();
		data.type = (Tile::Type)reader->UInt();
		Vector2 index = { floorf(data.pos.x / tileSize.x),	floorf(data.pos.y / tileSize.y) };
		ExpandBounds(index);

		Tile* tile = new Tile(data);
		tile->SetTag("FloorTile");
		tile->SetParent(this);

		RectCollider* col = tile->GetCollider();
		col->Scale() = { 1.0f, 0.85f };
		col->Pos().y -= (1.0f - col->Scale().y) * tile->GetSize().y;
		col->UpdateWorld();

		tile->Update();
		stairTiles[index] = tile;
	}
}

void SkulTileMap::LoadWallTiles(BinaryReader* reader)
{
	for (auto& pair : wallTiles) delete pair.second;
	wallTiles.clear();

	UINT size = reader->UInt();

	for (UINT i = 0; i < size; ++i)
	{
		Tile::Data data = {};

		data.textureFile = reader->WString();
		data.pos.x = reader->Float();
		data.pos.y = reader->Float();
		data.angle = reader->Float();
		data.type = (Tile::Type)reader->UInt();
		Vector2 index = { floorf(data.pos.x / tileSize.x),	floorf(data.pos.y / tileSize.y) };
		ExpandBounds(index);

		Tile* tile = new Tile(data);
		tile->SetTag("WallTile");
		tile->SetParent(this);

		tile->Update();
		wallTiles[index] = tile;
	}
}

void SkulTileMap::ExpandBounds(Vector2 index)
{
	minIndex.x = min(minIndex.x, index.x);
	minIndex.y = min(minIndex.y, index.y);
	maxIndex.x = max(maxIndex.x, index.x);
	maxIndex.y = max(maxIndex.y, index.y);
}

Vector2 SkulTileMap::WorldToGridPos(Vector2 worldPos)
{
	Vector2 local = worldPos - GlobalPos();
	local.x /= GlobalScale().x;
	local.y /= GlobalScale().y;

	Vector2 index = { floorf(local.x / tileSize.x), floorf(local.y / tileSize.y) };

	return index;
}
