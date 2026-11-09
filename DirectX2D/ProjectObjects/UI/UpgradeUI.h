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
	void OnThiefSkulUpgrade();

	void RenderRow(int row, Quad* icon, Button* button, const string& name, const string& levelText, int cost, bool canAfford);
private:
	static const int ROW_COUNT = 5;

	const float rowTopY = 70.0f;		
	const float rowGap = 48.0f;			
	const float iconX = -170.0f;		
	const float buttonX = 100.0f;		
	const float costIconX = 136.0f;		
	const float costIconScale = 1.5f;

	const Vector2 nameTextOffset = Vector2(160.0f, 0.0f);	
	const Vector2 levelTextOffset = Vector2(10.0f, 0.0f);	
	const Vector2 costTextOffset = Vector2(54.0f, 0.0f);	

	Quad* background = nullptr;

	Quad* healthIcon = nullptr;
	Quad* physicAttackIcon = nullptr;
	Quad* magicAttackIcon = nullptr;
	Quad* critChanceIcon = nullptr;
	Quad* thiefSkulUpgradeIcon = nullptr;

	Button* healthButton = nullptr;
	Button* physicAttackButton = nullptr;
	Button* magicAttackButton = nullptr;
	Button* critChanceButton = nullptr;
	Button* thiefSkulUpgradeButton = nullptr;

	Quad* costIcons[ROW_COUNT] = {};	 //각자 쓸 재화 아이콘

	const string titleTag = "강화 특성";
	const string healthButtonTag = "체력 증가 +20";
	const string physicAttackButtonTag = "물리 공격 증가 +2%";
	const string magicAttackButtonTag = "마법 공격 증가 +2%";
	const string critChanceButtonTag = "치명타 확률 증가 +1%";
	const string thiefSkulUpgradeButtonTag = "좀도둑 업그레이드";

	Player* player = nullptr;
};
