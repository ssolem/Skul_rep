#include "Framework.h"
#include "UpgradeUI.h"
#include "ProjectObjects/Character/PlayerUpgrade.h"
#include "ProjectObjects/Character/Player.h"

UpgradeUI::UpgradeUI(Player* player)
{
	this->player = player;

	background = new Quad(L"ProjectTextures/UI/Title/Etc/EA_Popup.png");
	background->Scale() = SKUL_DEFAULT_ASSET_SIZE;
	background->Pos() = Vector2(CENTER_X, CENTER_Y);

	Font::Get()->AddColor("Black", 0, 0, 0);
	Font::Get()->AddStyle("UpgradeTitle", L"맑은 고딕", 36, DWRITE_FONT_WEIGHT_BOLD);

	//아이콘
	healthIcon = new Quad(L"ProjectTextures/UI/StatUpgrade/Health.png");
	healthIcon->SetParent(background);
	healthIcon->Pos() = Vector2(-160, 58);

	physicAttackIcon = new Quad(L"ProjectTextures/UI/StatUpgrade/PhysicalAttack.png");
	physicAttackIcon->SetParent(background);
	physicAttackIcon->Pos() = Vector2(-160, 6);

	magicAttackIcon = new Quad(L"ProjectTextures/UI/StatUpgrade/MagicAttack.png");
	magicAttackIcon->SetParent(background);
	magicAttackIcon->Pos() = Vector2(-160, -46);

	critChanceIcon = new Quad(L"ProjectTextures/UI/StatUpgrade/CriticalChance.png");
	critChanceIcon->SetParent(background);
	critChanceIcon->Pos() = Vector2(-160, -98);

	//강화 버튼
	healthButton = new Button(L"Textures/Tile/tile.png");
	healthButton->SetParent(background);
	healthButton->Pos() = Vector2(160, 58);
	healthButton->Scale() = Vector2(0.5f, 0.5f);
	healthButton->SetEvent(std::bind(&UpgradeUI::OnHealthUpgrade, this));

	physicAttackButton = new Button(L"Textures/Tile/tile.png");
	physicAttackButton->SetParent(background);
	physicAttackButton->Pos() = Vector2(160, 6);
	physicAttackButton->Scale() = Vector2(0.5f, 0.5f);
	physicAttackButton->SetEvent(std::bind(&UpgradeUI::OnPhysicAttackUpgrade, this));

	magicAttackButton = new Button(L"Textures/Tile/tile.png");
	magicAttackButton->SetParent(background);
	magicAttackButton->Pos() = Vector2(160, -46);
	magicAttackButton->Scale() = Vector2(0.5f, 0.5f);
	magicAttackButton->SetEvent(std::bind(&UpgradeUI::OnMagicAttackUpgrade, this));

	critChanceButton = new Button(L"Textures/Tile/tile.png");
	critChanceButton->SetParent(background);
	critChanceButton->Pos() = Vector2(160, -98);
	critChanceButton->Scale() = Vector2(0.5f, 0.5f);
	critChanceButton->SetEvent(std::bind(&UpgradeUI::OnCritChanceUpgrade, this));

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
}

void UpgradeUI::Update()
{
	if (!isActive)
		return;

	if(KEY_DOWN(VK_ESCAPE))
	{
		CloseUI();
		return;
	}

	background->UpdateWorld();

	healthIcon->UpdateWorld();
	physicAttackIcon->UpdateWorld();
	magicAttackIcon->UpdateWorld();
	critChanceIcon->UpdateWorld();

	healthButton->Update();
	physicAttackButton->Update();
	magicAttackButton->Update();
	critChanceButton->Update();
}

void UpgradeUI::Render()
{
	if (!isActive)
		return;

	PlayerUpgrade* upgrade = player->GetUpgrade();

	background->Render();

	Vector2 titlePos = background->GlobalPos() + Vector2(0, 230);
	Font::Get()->RenderText(titleTag, { titlePos.x, titlePos.y }, { 160, 50 }, "UpgradeTitle", "White");

	healthIcon->Render();
	healthButton->Render();
	Vector2 textPos = healthIcon->GlobalPos() + Vector2(160, 0);
	Font::Get()->RenderText(healthButtonTag, { textPos.x, textPos.y }, { 220, 30 }, "Default", "White");
	textPos = healthButton->GlobalPos() + Vector2(10, 0);
	Font::Get()->RenderText("Lv." + to_string(upgrade->GetLevel(UpgradeType::Health)), { textPos.x, textPos.y }, { 60, 30 }, "Default", "Black");

	physicAttackIcon->Render();
	physicAttackButton->Render();
	textPos = physicAttackIcon->GlobalPos() + Vector2(160, 0);
	Font::Get()->RenderText(physicAttackButtonTag, { textPos.x, textPos.y }, { 220, 30 }, "Default", "White");
	textPos = physicAttackButton->GlobalPos() + Vector2(10, 0);
	Font::Get()->RenderText("Lv." + to_string(upgrade->GetLevel(UpgradeType::PhysicalAttack)), { textPos.x, textPos.y }, { 60, 30 }, "Default", "Black");

	magicAttackIcon->Render();
	magicAttackButton->Render();
	textPos = magicAttackIcon->GlobalPos() + Vector2(160, 0);
	Font::Get()->RenderText(magicAttackButtonTag, { textPos.x, textPos.y }, { 220, 30 }, "Default", "White");
	textPos = magicAttackButton->GlobalPos() + Vector2(10, 0);
	Font::Get()->RenderText("Lv." + to_string(upgrade->GetLevel(UpgradeType::MagicAttack)), { textPos.x, textPos.y }, { 60, 30 }, "Default", "Black");

	critChanceIcon->Render();
	critChanceButton->Render();
	textPos = critChanceIcon->GlobalPos() + Vector2(160, 0);
	Font::Get()->RenderText(critChanceButtonTag, { textPos.x, textPos.y }, { 220, 30 }, "Default", "White");
	textPos = critChanceButton->GlobalPos() + Vector2(10, 0);
	Font::Get()->RenderText("Lv." + to_string(upgrade->GetLevel(UpgradeType::CriticalChance)), { textPos.x, textPos.y }, { 60, 30 }, "Default", "Black");
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
