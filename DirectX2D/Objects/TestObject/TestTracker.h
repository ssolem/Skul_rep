#pragma once
class TestTracker : public Quad
{
private:
	const float MOVE_SPEED = 200.0f;
	const float ROT_SPEED = 5.0f;

public:
	TestTracker();
	TestTracker(bool useNodes);
	~TestTracker();

	void Update();
	void Render();
	void PostRender();

	vector<Vector2>& GetPath() { return path; }

	Collider* GetCollider() { return collider; }

private:
	void TrackPath();
	void TrackNodes();

	bool useNodes = false;

	vector<Vector2> path;

	Collider* collider;
};

