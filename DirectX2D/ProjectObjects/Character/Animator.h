#pragma once

class AtlasMaker;

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
		Vector2 scale = Vector2::One, float speed = 1.0f, bool isAdditive = false);

	Clip* FindClip(const string& key);
	void PlayClip(const string& key);

	static void ClearFrameCache();

	Vector2 GetCurFrameSize() { return curClip ? curClip->GetCurFrame()->GetSize() : Vector2::One; }

private:
	map<string, Clip*> clips;
	Clip* curClip;

	AtlasMaker* atlas;

	static map<wstring, vector<Frame*>> frameCache;
};

