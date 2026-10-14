#pragma once

class Animator;

class AnimObject : public GameObject
{
public:
	AnimObject();
	~AnimObject();

	void Update();
	void Render();

	void AddClip(const string& fileName, const wstring& folder, bool isLoop = false, float speed = 1.0f);
	Clip* FindClip(const string& key);

	void Play(const string& key, Vector2 pos);	
	void Stop() { SetActive(false); }

private:
	Animator* animator;
};
