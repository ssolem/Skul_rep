#include "Framework.h"

EditTileMap::EditTileMap(UINT width, UINT height)
	: width(width), height(height)
{
	CreateTile();
}

EditTileMap::~EditTileMap()
{
	for(Tile* tile : bgTiles) delete tile;
	for(Tile* tile : objTiles) delete tile;
	for(Tile* tile : bushTiles) delete tile;
}

void EditTileMap::Update()
{
	for (Tile* tile : bgTiles) tile->Update();
	for (Tile* tile : objTiles) tile->Update();
	for (Tile* tile : bushTiles) tile->Update();

	UpdateWorld();
}

void EditTileMap::Render()
{
	for (Tile* tile : bgTiles) tile->Render();
	for (Tile* tile : objTiles) tile->Render();
	for (Tile* tile : bushTiles) tile->Render();
}	

void EditTileMap::ClickTile(Quad* selectSample, Tile::Type type)
{
	//들고 있던 샘플을 각각 타입에 맞게 적용, 변경

	switch (type)
	{
	case Tile::Type::BG:
		SetBGTile(selectSample->GetTexture()->GetFile(), selectSample->Rot().z);
		break;
	case Tile::Type::FLOOR:
		SetObjTile(selectSample->GetTexture()->GetFile(), selectSample->Rot().z);
		break;
	case Tile::Type::BUSH:
		SetBushTile(selectSample->GetTexture()->GetFile(), selectSample->Rot().z);
		break;
	}
}

void EditTileMap::DeleteTile(Tile::Type type)
{
	//무엇에 대한 삭제인지 대상을 지정

	vector<Tile*>* target = nullptr;

	switch (type)
	{
	case Tile::Type::FLOOR: target = &objTiles;
		break;
	case Tile::Type::BUSH: target = &bushTiles;
		break;
	}

	if (!target) return;

	//이어서 해당 벡터를 순회하기 위한 반복자 지정
	vector<Tile*>::iterator iter = target->begin();

	for(iter; iter != target->end();)
	{
		//종료 시의 변화(변화절) 없음 : 변화를 이 안에서 직접 지정해줘야 한다.
		
		if((*iter)->GetCollider()->IsPointCollision(mousePos))
		{
			delete *iter; //타일 삭제
			iter = target->erase(iter); //벡터에서 제거
		}
		else
			iter++;
	}
}

void EditTileMap::Save(string file)
{
	//winAPI 및 DXWrite의 기능을 호출해 텍스트 파일을 작성, 호출하는 과정

	//두 기능을 호출하는 클래스 '바이너리 라이터' 호출
	BinaryWriter* writer = new BinaryWriter(file);

	//일단 파일이 만들어짐 - 여기에 차례로 데이터를 byte표기로 작성

	writer->UInt(width);
	writer->UInt(height);

	writer->UInt(bgTiles.size());

	for (Tile* tile : bgTiles)
	{
		Tile::Data data = tile->GetData();		//각 타일마다 데이터를 준비해서

		writer->WString(data.textureFile); //그림 파일이 뭔지
		writer->Float(data.pos.x);			//위치
		writer->Float(data.pos.y);
		writer->Float(data.angle);
		writer->UInt((int)data.type);
	}

	writer->UInt(objTiles.size());

	for (Tile* tile : objTiles)
	{
		Tile::Data data = tile->GetData();	

		writer->WString(data.textureFile); 
		writer->Float(data.pos.x);			
		writer->Float(data.pos.y);
		writer->Float(data.angle);
		writer->UInt((int)data.type);
	}

	writer->UInt(bushTiles.size());

	for (Tile* tile : bushTiles)
	{
		Tile::Data data = tile->GetData();

		writer->WString(data.textureFile);
		writer->Float(data.pos.x);
		writer->Float(data.pos.y);
		writer->Float(data.angle);
		writer->UInt((int)data.type);
	}

	delete writer;
}

void EditTileMap::Load(string file)
{
	BinaryReader* reader = new BinaryReader(file);

	width = reader->UInt();
	height = reader->UInt();

	UINT size = reader->UInt();

	for (Tile* tile : bgTiles)
	{
		Tile::Data data = tile->GetData();		

		data.textureFile = reader->WString();
		data.pos.x = reader->Float();
		data.pos.y = reader->Float();
		data.angle = reader->Float();
		data.type = (Tile::Type)reader->UInt();

		tile->SetTexture(data.textureFile);
		tile->Pos() = data.pos;
		tile->Rot().z = data.angle;
	}
	// 세이브된 배경판만큼이 벡터에 갱신

	//오브젝트, 부쉬의 경우 벡터의 전체 갱신이 필요하다.
	//->벡터를 처음부터 추가

	for (Tile* tile : objTiles) delete tile; // 벡터 안의 내용물을 다 지우고

	size = reader->UInt();
	objTiles.resize(size);

	//로딩된 벡터에 원래 개수만큼의 원소공간이 할당 됐으니 그 타일이 무엇인지만 정해주면 된다.
	for (Tile*& tile : objTiles)
	{
		Tile::Data data; //생성을 위한 데이터

		data.textureFile = reader->WString(); 
		data.pos.x = reader->Float();			
		data.pos.y = reader->Float();
		data.angle = reader->Float();
		data.type = (Tile::Type)reader->UInt();

		//로딩이 진행된 데이터로 타일 생성
		tile = new Tile(data);
		tile->SetParent(this);
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

		//로딩이 진행된 데이터로 타일 생성
		tile = new Tile(data);
		tile->SetParent(this);
	}

	delete reader;
}

void EditTileMap::CreateTile()
{	
	//판의 기본배경 지정
	wstring baseTile = L"Textures/Tile/tile.png";
	Texture* texture = Texture::Add(baseTile);

	tileSize = texture->GetSize();

	//기본판 배경 생성

	for (UINT y = 0; y < height; y++)
	{
		for (UINT x = 0; x < width; x++)
		{
			//기본 판 정보 생성
			Tile::Data data = {};
			data.textureFile = baseTile;
			data.pos = Vector2(x * tileSize.x, y * tileSize.y);

			Tile* tile = new Tile(data);
			tile->SetParent(this); //타일들이 판 따라 움직이도록
			bgTiles.push_back(tile);
		}
	}
}

void EditTileMap::SetBGTile(wstring file, float angle)
{
	// 배경을 클릭해서 갱신하는 경우

	for(Tile* tile : bgTiles)
	{
		// 마우스 위치에 있는 타일을 찾음
		if(tile->GetCollider()->IsPointCollision(mousePos))
		{
			tile->SetTexture(file);
			tile->SetAngle(angle);
		}
	}
}

void EditTileMap::SetObjTile(wstring file, float angle)
{
	// 오브젝트를 다른것으로 바꾸는것

	DeleteTile(Tile::Type::FLOOR);

	for(Tile* tile : bgTiles)
	{
		if(tile->GetCollider()->IsPointCollision(mousePos))
		{
			Tile::Data data = tile->GetData(); //해당 좌표 타일 가져오기
			data.angle = angle;
			data.textureFile = file;
			data.type = Tile::Type::FLOOR;

			//가져온 정보로 새 오브젝트 타일 생성
			Tile* tile = new Tile(data);
			tile->SetParent(this);
			tile->UpdateWorld();
			objTiles.push_back(tile);
		}
	}
}

void EditTileMap::SetBushTile(wstring file, float angle)
{
	// 험지를 다른것으로 바꾸는것
	DeleteTile(Tile::Type::BUSH);
	for(Tile* tile : bgTiles)
	{
		if(tile->GetCollider()->IsPointCollision(mousePos))
		{
			Tile::Data data = tile->GetData(); //해당 좌표 타일 가져오기
			data.angle = angle;
			data.textureFile = file;
			data.type = Tile::Type::BUSH;
			//가져온 정보로 새 험지 타일 생성
			Tile* tile = new Tile(data);
			tile->SetParent(this);
			tile->UpdateWorld();
			bushTiles.push_back(tile);
		}
	}
}
