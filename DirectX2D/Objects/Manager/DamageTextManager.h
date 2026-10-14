#pragma once

class DamageTextManager : public Singleton<DamageTextManager>
{
private:
	friend class Singleton;

	DamageTextManager();
	~DamageTextManager();

public:
	void Update();
	void Render();

	void Play(Vector2 worldPos, int amount, DamageType type, bool cirtical = false, bool isPlayer = false);
	void Clear();

private:
	struct DamageText
	{
		int damage;
		Vector2 worldPos;
		DamageType type;
		float time = 0.0f;
		bool isCritical = false;
		bool isPlayer = false;
		bool isActive = false;
	};

	const float LIFE_TIME = 0.8f;
	const float RISE_SPEED = 80.0f;
	const float FADE_START = 0.6f;

	vector<DamageText> texts;
};

