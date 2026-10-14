#pragma once

class GameTileMap;
class Tank;

class TestQuad;

class TankScene : public Scene
{
public:
	TankScene();
	~TankScene();

	virtual void Update() override;
	virtual void Render() override;
	virtual void PostRender() override;

	virtual void Start() override;
	virtual void End() override;
	virtual void Reset() override;

private:
	GameTileMap* map;
	AStar* aStar;

	Tank* tank;
	TestQuad* quad;

	ImageFont* imageFont;
};

