#include "Framework.h"
#include "SkulEditTileMap.h"


SkulEditTileMap::SkulEditTileMap()
{
}

SkulEditTileMap::~SkulEditTileMap()
{
	for (auto& pair : bgTiles) delete pair.second;
	for (auto& pair : floorTiles) delete pair.second;
	for (auto& pair : stairTiles) delete pair.second;
	for (auto& pair : wallTiles) delete pair.second;
}

void SkulEditTileMap::Update()
{
	for (auto& pair : bgTiles) pair.second->Update();
	for (auto& pair : floorTiles) pair.second->Update();
	for (auto& pair : stairTiles) pair.second->Update();
	for (auto& pair : wallTiles) pair.second->Update();

	UpdateWorld();
}

void SkulEditTileMap::Render()
{
	for (auto& pair : bgTiles) pair.second->Render();
	for (auto& pair : floorTiles) pair.second->Render();
	for (auto& pair : stairTiles) pair.second->Render();
	for (auto& pair : wallTiles) pair.second->Render();
}

void SkulEditTileMap::SetTileScale(Vector2 scale)
{
	tileScale = scale;
	Scale() = tileScale;
	SetRenderSize();
	UpdateWorld();
}

void SkulEditTileMap::PlaceTile(Quad* selectSample, Tile::Type type)
{
	if (selectSample == nullptr || selectSample->GetTexture() == nullptr)
		return;

	Vector2 index = GetTileIndex();
	wstring file = selectSample->GetTexture()->GetFile();

	switch (type)
	{
	case Tile::Type::BG:
		if (bgTiles.find(index) != bgTiles.end())
		{
			SetBGTile(file, index, selectSample->Rot().z);
			return;
		}
		break;
	case Tile::Type::FLOOR:
		if (floorTiles.find(index) != floorTiles.end())
		{
			SetFloorTile(file, index, selectSample->Rot().z);
			return;
		}
		break;
	case Tile::Type::STAIR:
		if (stairTiles.find(index) != stairTiles.end())
		{
			SetStairTile(file, index, selectSample->Rot().z);
			return;
		}
	case Tile::Type::WALL:
		if (wallTiles.find(index) != wallTiles.end())
		{
			SetWallTile(file, index, selectSample->Rot().z);
			return;
		}
		break;
	}

	CreateTile(file, index, selectSample->Rot().z, type);
}

void SkulEditTileMap::EraseTile(Tile::Type type)
{
	Vector2 index = GetTileIndex();

	map<Vector2, Tile*, Vector2IntLess>* tiles = nullptr;
	switch (type)
	{
	case Tile::Type::BG:
		tiles = &bgTiles;
		break;
	case Tile::Type::FLOOR:
		tiles = &floorTiles;
		break;
	case Tile::Type::STAIR:
		tiles = &stairTiles;
		break;
	case Tile::Type::WALL:
		tiles = &wallTiles;
	}

	if (tiles && tiles->find(index) != tiles->end())
	{
		delete (*tiles)[index];
		tiles->erase(index);
	}

	RecalculateTileMapSize();
}

void SkulEditTileMap::SelectBaseTile()
{
	if (ImGui::Button("Select Base Tile"))
		DIALOG->OpenDialog("BaseTileSelect", "Select File", ".png", ".");

	if (DIALOG->Display("BaseTileSelect"))
	{
		if (DIALOG->IsOk())
		{
			string file = DIALOG->GetFilePathName();
			Texture* texture = Texture::Add(ToWString(file));

			SetTileSize(texture->GetSize());
		}

		DIALOG->Close();
	}
}

void SkulEditTileMap::Save(string file)
{
	RecalculateTileMapSize();

	width = (UINT)(rightTop.x - leftBottom.x + 1);
	height = (UINT)(rightTop.y - leftBottom.y + 1);

	BinaryWriter* writer = new BinaryWriter(file);

	writer->UInt(width);
	writer->UInt(height);
	writer->Float(tileSize.x);
	writer->Float(tileSize.y);

	SaveTiles(writer, bgTiles);
	SaveTiles(writer, floorTiles);
	SaveTiles(writer, stairTiles);
	SaveTiles(writer, wallTiles);

	delete writer;
}

void SkulEditTileMap::Load(string file)
{
	BinaryReader* reader = new BinaryReader(file);

	width = reader->UInt();
	height = reader->UInt();
	tileSize.x = (int)reader->Float();
	tileSize.y = (int)reader->Float();

	LoadTiles(reader, bgTiles);
	LoadTiles(reader, floorTiles);
	LoadTiles(reader, stairTiles);
	LoadTiles(reader, wallTiles);

	delete reader;

	RecalculateTileMapSize();
}

void SkulEditTileMap::SetBGTile(wstring file, Vector2 index, float angle)
{
	bgTiles[index]->SetTexture(file);
	bgTiles[index]->SetAngle(angle);
}

void SkulEditTileMap::SetFloorTile(wstring file, Vector2 index, float angle)
{
	floorTiles[index]->SetTexture(file);
	floorTiles[index]->SetAngle(angle);
}

void SkulEditTileMap::SetStairTile(wstring file, Vector2 index, float angle)
{
	stairTiles[index]->SetTexture(file);
	stairTiles[index]->SetAngle(angle);
}

void SkulEditTileMap::SetWallTile(wstring file, Vector2 index, float angle)
{
	wallTiles[index]->SetTexture(file);
	wallTiles[index]->SetAngle(angle);
}

void SkulEditTileMap::CreateTile(wstring file, Vector2 index, float angle, Tile::Type type)
{
	Tile::Data data = {};
	data.textureFile = file;
	data.pos = Vector2(index.x * tileSize.x + tileSize.x /2.0f, index.y * tileSize.y + tileSize.y /2.0f);
	data.angle = angle;
	data.type = type;

	Tile* tile = new Tile(data);
	tile->SetParent(this);
	tile->UpdateWorld();

	switch (type)
	{
	case Tile::Type::BG:
		bgTiles[index] = tile;
		break;
	case Tile::Type::FLOOR:
		floorTiles[index] = tile;
		break;
	case Tile::Type::STAIR:
		stairTiles[index] = tile;
		break;
	case Tile::Type::WALL:
		wallTiles[index] = tile;
		break;
	}

	RecalculateTileMapSize();
}

void SkulEditTileMap::RecalculateTileMapSize()
{
	bool init = false;
	Vector2 lb{}, rt{};

	auto scan = [&](map<Vector2, Tile*, Vector2IntLess>& tiles)
		{
			for (auto& pair : tiles)
			{
				Vector2 p = pair.first;
				if (!init) { lb = rt = p; init = true; continue; }
				if (p.x < lb.x) lb.x = p.x;
				if (p.y < lb.y) lb.y = p.y;
				if (p.x > rt.x) rt.x = p.x;
				if (p.y > rt.y) rt.y = p.y;
			}
		};
	scan(bgTiles); scan(floorTiles); scan(stairTiles); scan(wallTiles);

	leftBottom = Vector2((float)lb.x, (float)lb.y);
	rightTop = Vector2((float)rt.x, (float)rt.y);
}

void SkulEditTileMap::SaveTiles(BinaryWriter* writer, map<Vector2, Tile*, Vector2IntLess>& tiles)
{
	writer->UInt(tiles.size());
	for (auto& pair : tiles)
	{
		Tile::Data data = pair.second->GetData();

		writer->WString(data.textureFile);
		writer->Float(data.pos.x);
		writer->Float(data.pos.y);
		writer->Float(data.angle);
		writer->UInt((int)data.type);
	}
}

void SkulEditTileMap::LoadTiles(BinaryReader* reader, map<Vector2, Tile*, Vector2IntLess>& tiles)
{
	for (auto& pair : tiles) delete pair.second;
	tiles.clear();

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
		Tile* tile = new Tile(data);
		tile->SetParent(this);
		tiles[index] = tile;
	}
}