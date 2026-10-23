#pragma once

class Player;
class Enemy;
class Stage;
class PlayerInfoUI;
class StageBackground;

class TestCharacterScene : public Scene
{
public:
	TestCharacterScene();
	~TestCharacterScene();

	virtual void Update() override;
	virtual void Render() override;
	virtual void PostRender() override;

	virtual void Start() override;
	virtual void End() override;
	virtual void Reset() override;

private:
	void BuildStage();
	void ClearStage();

private:
	Player* player;
	Stage* stage = nullptr;
	StageBackground* background;

	PlayerInfoUI* playerInfoUI;
};

