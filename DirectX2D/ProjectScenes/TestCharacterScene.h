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
	Player* player;
	Enemy* ent;
	Enemy* ent2;
	Enemy* ent3;
	Enemy* ent4;
	Stage* stage;
	StageBackground* background;

	PlayerInfoUI* playerInfoUI;
};

