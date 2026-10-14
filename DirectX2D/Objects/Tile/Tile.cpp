#include "Framework.h"

Tile::Tile(Data data) 
	: Button(data.textureFile), data(data)
{
	//실제 위치를 데이터에서 가져와 반영
	Pos() = data.pos;
	Rot().z = data.angle;
}	

Tile::~Tile()
{
}

void Tile::SetTexture(wstring file)
{
	Quad::SetTexture(file);
	data.textureFile = file;
}

void Tile::SetAngle(float angle)
{
	Rot().z = angle;
	data.angle = angle;
}
