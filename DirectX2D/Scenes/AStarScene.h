#pragma once

class TestTracker;
class GameTileMap;

class AStarScene : public Scene
{
public:

	AStarScene();
	~AStarScene();

	virtual void Update() override;
	virtual void Render() override;
	virtual void PostRender() override;

	virtual void Start() override;
	virtual void End() override;
	virtual void Reset() override;

private:

	GameTileMap* map;
	TestTracker* tracker;
	AStar* aStar;
};

