#pragma once

class EffectManager : public Singleton<EffectManager>
{
private:
	friend class Singleton;

	EffectManager() = default;
	~EffectManager();

public:
	void Update();
	void Render();

	void Add(string key, UINT poolSize, wstring textureFile,
		float maxFrameX, float maxFrameY, float speed = 1,
		bool isAdditive = false);

	bool HasEffect(const string& key) { return totalEffects.count(key) > 0; }

	Effect* Play(string key, Vector2 pos, bool flipX = true);
	void SetColor(string key, float r, float g, float b, float a = 1);
	void SetScale(string key, Vector2 scale);
	void SetFrameDelay(string key, UINT frame, float delay);

private:
	Effect* Expand(const string& key);	//풀이 다 찼을 때 같은 설정으로 하나 추가

private:
	//풀 확장 시 같은 이펙트를 다시 만들기 위한 생성 정보
	struct EffectDesc
	{
		wstring textureFile;
		Vector2 maxFrame;
		float speed;
		bool isAdditive;
	};

	typedef vector<Effect*> Effects;
	unordered_map<string, Effects> totalEffects;
	unordered_map<string, EffectDesc> descs;

};

