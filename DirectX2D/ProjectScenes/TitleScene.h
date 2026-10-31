#pragma once

class TitleScene : public Scene
{
public:
	TitleScene();
	~TitleScene();

	virtual void Update() override;
	virtual void Render() override;
	virtual void PostRender() override;

	virtual void Start() override;
	virtual void End() override;
	virtual void Reset() override;

private:
	bool IsAnyKeyDown();
	bool IsExceptionKey(UINT key);

private:

	Quad* background = nullptr;
	Quad* logo = nullptr;

	float time = 0.0f;
	float blinkSpeed = 3.0f;
};
