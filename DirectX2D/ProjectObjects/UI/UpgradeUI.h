#pragma once
class Player;

class UpgradeUI : public GameObject
{
public:
	UpgradeUI(Player* player);
	~UpgradeUI();

	void Update();
	void Render();

	void OpenUI();
	bool IsOpen() { return isActive; }

private:
	void CloseUI();

	void OnHealthUpgrade();
	void OnPhysicAttackUpgrade();
	void OnMagicAttackUpgrade();
	void OnCritChanceUpgrade();

private:

	Quad* background = nullptr;
	Button* healthButton = nullptr;
	Button* physicAttackButton = nullptr;
	Button* magicAttackButton = nullptr;
	Button* critChanceButton = nullptr;

	const string healthButtonTag = "체력 증가 +20";
	const string physicAttackButtonTag = "물리 공격 증가 +2%";
	const string magicAttackButtonTag = "마법 공격 증가 +2%";
	const string critChanceButtonTag = "치명타 확률 증가 +1%";

	int healthCount = 0;
	int physicAttackCount = 0;
	int magicAttackCount = 0;
	int critChanceCount = 0;

	Player* player = nullptr;
};

