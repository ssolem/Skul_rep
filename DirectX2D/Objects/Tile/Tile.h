#pragma once

class Tile : public Button
{
public:

	enum class Type
	{
		BG,
		FLOOR,
		STAIR,
		WALL,
		BUSH,
	};

	struct Data
	{
		wstring textureFile;
		Vector2 pos;
		float angle;
		Type type = Type::BG;
	};

public:
	Tile(Data data);
	~Tile();

	void SetTexture(wstring file);
	void SetAngle(float angle);

	Data GetData() { return data; }

private:
	Data data;

};
