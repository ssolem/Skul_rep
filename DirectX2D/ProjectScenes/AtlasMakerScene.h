#pragma once

class AtlasMaker;

class AtlasMakerScene : public Scene
{
public:
	AtlasMakerScene();
	~AtlasMakerScene();

	virtual void Update() override;
	virtual void Render() override;
	virtual void PostRender() override;

	virtual void Start() override;
	virtual void End() override;
	virtual void Reset() override;

private:
	void CreateAtlas();
	void CreateEffect();
	void Save();

private:
	AtlasMaker* atlasMaker;
	int optionIndex;
	string projectPath;

	Quad* sample = nullptr;
	bool hasSample = false;
};

