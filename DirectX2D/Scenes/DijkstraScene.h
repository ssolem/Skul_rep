#pragma once

class TestTracker;

class DijkstraScene : public Scene
{
public:
	DijkstraScene();
	~DijkstraScene();

	virtual void Update() override;
	virtual void Render() override;
	virtual void PostRender() override;

private:
	Dijkstra* dijkstra;
	TestTracker* tracker;
};

