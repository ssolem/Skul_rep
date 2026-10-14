#pragma once

class StageBackground : public Transform
{
public:
	StageBackground();
	~StageBackground();

	void Update();
	void Render();

	void AddLayer(wstring file, float moveSpeed = 0.0f);
	void Reset();

private:
	struct Layer
	{
		vector<Quad*> copies;
		float moveSpeed = 0.0f;
		float offsetX = 0.0f;
		float offsetY = 0.0f;
		float width = 0.0f;
	};

private:
	vector<Layer> layers;
	vector<float> moveSpeeds;	//각 레이어별 이동 속도 (0이면 고정)

	Vector2 camPrevPos = Vector2::Zero;

};

