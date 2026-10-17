#pragma once

class Animator
{
public:
	Animator();
	~Animator();

	void Update();
	void Render();

	void CreateClip(const string& key, const wstring& path, vector<Frame*>& frames, bool isLoop = true, float speed = 1.0f);
	void CreateClipByFolder(const string& key, const wstring& path, bool isLoop = true, float speed = 1.0f);
	void CreateClipByAtlas(const string& tag, const string& fileName, const wstring& path, bool isLoop = true, float speed = 1.0f);
	void CreateEffectByAtlas(const string& key, const wstring& atlasFile, UINT poolSize,
		Vector2 scale = Vector2::One, float speed = 1.0f, bool isAdditive = false, EffectPivot pivot = EffectPivot::Center);

	Clip* FindClip(const string& key);
	void PlayClip(const string& key);

	//아틀라스(.png + .uv)의 프레임 목록 - 경로+피벗별로 한 번만 만들고 공유 (소유는 캐시)
	//파일이 없으면 nullptr
	static vector<Frame*>* GetAtlasFrames(const wstring& atlasPath, bool isCenterPivot = false);

	static void ClearFrameCache();

	Vector2 GetCurFrameSize() { return curClip ? curClip->GetCurFrame()->GetSize() : Vector2::One; }

private:
	map<string, Clip*> clips;
	Clip* curClip;

	static map<wstring, vector<Frame*>> frameCache;
};

