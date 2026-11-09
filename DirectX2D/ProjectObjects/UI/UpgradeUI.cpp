#include "Framework.h"
#include "UpgradeUI.h"
#include "ProjectObjects/Character/PlayerUpgrade.h"
#include "ProjectObjects/Character/Player.h"
#include "ProjectObjects/Character/Skuls/ThiefSkul.h"

UpgradeUI::UpgradeUI(Player* player)
{
	this->player = player;

	background = new Quad(L"ProjectTextures/UI/Title/Etc/EA_Popup.png");
	background->Scale() = SKUL_DEFAULT_ASSET_SIZE;
	background->Pos() = Vector2(CENTER_X, CENTER_Y);

	Font::Get()->AddColor("Black", 0, 0, 0);
	Font::Get()->AddColor("Red", 1.0f, 0.3f, 0.3f);
	Font::Get()->AddStyle("UpgradeTitle", L"맑은 고딕", 36, DWRITE_FONT_WEIGHT_BOLD);

	//아이콘
	healthIcon = new Quad(L"ProjectTextures/UI/StatUpgrade/Health.png");
	physicAttackIcon = new Quad(L"ProjectTextures/UI/StatUpgrade/PhysicalAttack.png");
	magicAttackIcon = new Quad(L"ProjectTextures/UI/StatUpgrade/MagicAttack.png");
	critChanceIcon = new Quad(L"ProjectTextures/UI/StatUpgrade/CriticalChance.png");
	thiefSkulUpgradeIcon = new Quad(L"ProjectTextures/UI/StatUpgrade/ThiefSkul_Upgrade.png");

	//강화 버튼
	healthButton = new Button(L"Textures/Tile/tile.png");
	healthButton->SetEvent(std::bind(&UpgradeUI::OnHealthUpgrade, this));

	physicAttackButton = new Button(L"Textures/Tile/tile.png");
	physicAttackButton->SetEvent(std::bind(&UpgradeUI::OnPhysicAttackUpgrade, this));

	magicAttackButton = new Button(L"Textures/Tile/tile.png");
	magicAttackButton->SetEvent(std::bind(&UpgradeUI::OnMagicAttackUpgrade, this));

	critChanceButton = new Button(L"Textures/Tile/tile.png");
	critChanceButton->SetEvent(std::bind(&UpgradeUI::OnCritChanceUpgrade, this));

	thiefSkulUpgradeButton = new Button(L"Textures/Tile/tile.png");
	thiefSkulUpgradeButton->SetEvent(std::bind(&UpgradeUI::OnThiefSkulUpgrade, this));

	const wstring costPath = L"ProjectTextures/Common/UI/";
	auto CurrencyIconFile = [&](UpgradeType type)
	{
		return PlayerUpgrade::GetInfo(type).currency == CurrencyType::Gold ? L"Gold_Icon.png" : L"DarkQuartz_Icon.png";
	};

	costIcons[0] = new Quad(costPath + CurrencyIconFile(UpgradeType::Health));
	costIcons[1] = new Quad(costPath + CurrencyIconFile(UpgradeType::PhysicalAttack));
	costIcons[2] = new Quad(costPath + CurrencyIconFile(UpgradeType::MagicAttack));
	costIcons[3] = new Quad(costPath + CurrencyIconFile(UpgradeType::CriticalChance));
	costIcons[4] = new Quad(costPath + L"BoneChip_Icon.png");

	Quad* icons[ROW_COUNT] = { healthIcon, physicAttackIcon, magicAttackIcon, critChanceIcon, thiefSkulUpgradeIcon };
	Button* buttons[ROW_COUNT] = { healthButton, physicAttackButton, magicAttackButton, critChanceButton, thiefSkulUpgradeButton };

	for (int i = 0; i < ROW_COUNT; i++)
	{
		float y = rowTopY - rowGap * i;

		icons[i]->SetParent(background);
		icons[i]->Pos() = Vector2(iconX, y);

		buttons[i]->SetParent(background);
		buttons[i]->Pos() = Vector2(buttonX, y);
		buttons[i]->Scale() = Vector2(0.5f, 0.5f);

		costIcons[i]->SetParent(background);
		costIcons[i]->Pos() = Vector2(costIconX, y);
		costIcons[i]->Scale() = Vector2(costIconScale, costIconScale);
	}

	isActive = false;
}

UpgradeUI::~UpgradeUI()
{
	delete background;
	delete healthIcon;
	delete physicAttackIcon;
	delete magicAttackIcon;
	delete critChanceIcon;
	delete healthButton;
	delete physicAttackButton;
	delete magicAttackButton;
	delete critChanceButton;
	delete thiefSkulUpgradeIcon;
	delete thiefSkulUpgradeButton;

	for (Quad* costIcon : costIcons)
		delete costIcon;
}

void UpgradeUI::Update()
{
	if (!isActive)
		return;

	if (KEY_DOWN(VK_ESCAPE))
	{
		CloseUI();
		return;
	}

	background->UpdateWorld();

	healthIcon->UpdateWorld();
	physicAttackIcon->UpdateWorld();
	magicAttackIcon->UpdateWorld();
	critChanceIcon->UpdateWorld();
	thiefSkulUpgradeIcon->UpdateWorld();

	for (Quad* costIcon : costIcons)
		costIcon->UpdateWorld();

	healthButton->Update();
	physicAttackButton->Update();
	magicAttackButton->Update();
	critChanceButton->Update();
	thiefSkulUpgradeButton->Update();
}

void UpgradeUI::Render()
{
	if (!isActive)
		return;

	PlayerUpgrade* upgrade = player->GetUpgrade();

	background->Render();

	Vector2 titlePos = background->GlobalPos() + Vector2(0, 230);
	Font::Get()->RenderText(titleTag, { titlePos.x, titlePos.y }, { 160, 50 }, "UpgradeTitle", "White");

	Quad* statIcons[] = { healthIcon, physicAttackIcon, magicAttackIcon, critChanceIcon };
	Button* statButtons[] = { healthButton, physicAttackButton, magicAttackButton, critChanceButton };
	const string* statNames[] = { &healthButtonTag, &physicAttackButtonTag, &magicAttackButtonTag, &critChanceButtonTag };

	for (int i = 0; i < (int)UpgradeType::Max; i++)
	{
		UpgradeType type = (UpgradeType)i;
		int cost = upgrade->IsMaxLevel(type) ? 0 : upgrade->GetCost(type);

		RenderRow(i, statIcons[i], statButtons[i], *statNames[i],
			"Lv." + to_string(upgrade->GetLevel(type)), cost, upgrade->CanUpgrade(type));
	}

	string thiefLevelText = "-";
	int thiefCost = 0;
	if (ThiefSkul* thief = dynamic_cast<ThiefSkul*>(player->GetCurSkul()))
	{
		thiefLevelText = thief->IsMaxTier() ? "MAX" : "Lv." + to_string(thief->GetTier());
		thiefCost = thief->GetUpgradeCost();
	}
	RenderRow(4, thiefSkulUpgradeIcon, thiefSkulUpgradeButton, thiefSkulUpgradeButtonTag,
		thiefLevelText, thiefCost, thiefCost > 0 && player->GetBoneChip() >= thiefCost);
}

void UpgradeUI::RenderRow(int row, Quad* icon, Button* button, const string& name, const string& levelText, int cost, bool canAfford)
{
	icon->Render();
	button->Render();
	costIcons[row]->Render();

	Vector2 textPos = icon->GlobalPos() + nameTextOffset;
	Font::Get()->RenderText(name, { textPos.x, textPos.y }, { 220, 30 }, "Default", "White");

	textPos = button->GlobalPos() + levelTextOffset;
	Font::Get()->RenderText(levelText, { textPos.x, textPos.y }, { 60, 30 }, "Default", "Black");

	//재화가 모자라면 빨간색
	textPos = costIcons[row]->GlobalPos() + costTextOffset;
	string costText = cost > 0 ? to_string(cost) : "-";
	Font::Get()->RenderText(costText, { textPos.x, textPos.y }, { 80, 30 }, "Default", canAfford ? "White" : "Red");
}

void UpgradeUI::OpenUI()
{
	isActive = true;
}

void UpgradeUI::CloseUI()
{
	isActive = false;
}

void UpgradeUI::OnHealthUpgrade()
{
	player->GetUpgrade()->TryUpgrade(UpgradeType::Health);
}

void UpgradeUI::OnPhysicAttackUpgrade()
{
	player->GetUpgrade()->TryUpgrade(UpgradeType::PhysicalAttack);
}

void UpgradeUI::OnMagicAttackUpgrade()
{
	player->GetUpgrade()->TryUpgrade(UpgradeType::MagicAttack);
}

void UpgradeUI::OnCritChanceUpgrade()
{
	player->GetUpgrade()->TryUpgrade(UpgradeType::CriticalChance);
}

void UpgradeUI::OnThiefSkulUpgrade()
{
	ThiefSkul* thief = dynamic_cast<ThiefSkul*>(player->GetCurSkul());
	if (thief == nullptr || thief->IsMaxTier())
		return;

	int cost = thief->GetUpgradeCost();
	if (player->GetBoneChip() < cost)
		return;

	player->AddBoneChip(-cost);
	thief->UpgradeTier();

	player->RefreshSkulUI();
}
