#pragma once

class Ninja;
class GameTileMap;

class TestMapScene : public Scene
{
public:
	TestMapScene();
	~TestMapScene();

	virtual void Update() override;
	virtual void Render() override;
	virtual void PostRender() override;

private:
	GameTileMap* map;

	Ninja* player;
};

