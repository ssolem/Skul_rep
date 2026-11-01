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

	healthButton = new Button(L"ProjectTextures/UI/StatUpgrade/Health.png");
	healthButton->SetParent(background);
	healthButton->Pos() = Vector2();
	healthButton->SetEvent(std::bind(&UpgradeUI::OnHealthUpgrade, this));

	physicAttackButton = new Button(L"ProjectTextures/UI/StatUpgrade/PhysicalAttack.png");
	physicAttackButton->SetParent(background);
	physicAttackButton->Pos() = Vector2(0, -50);
	physicAttackButton->SetEvent(std::bind(&UpgradeUI::OnPhysicAttackUpgrade, this));

	magicAttackButton = new Button(L"ProjectTextures/UI/StatUpgrade/MagicAttack.png");
	magicAttackButton->SetParent(background);
	magicAttackButton->Pos() = Vector2(0, -100);
	magicAttackButton->SetEvent(std::bind(&UpgradeUI::OnMagicAttackUpgrade, this));

	critChanceButton = new Button(L"ProjectTextures/UI/StatUpgrade/CriticalChance.png");
	critChanceButton->SetParent(background);
	critChanceButton->Pos() = Vector2(0, -150);
	critChanceButton->SetEvent(std::bind(&UpgradeUI::OnCritChanceUpgrade, this));

	isActive = false;
}

UpgradeUI::~UpgradeUI()
{
	delete background;
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
	healthButton->Update();
	physicAttackButton->Update();
	magicAttackButton->Update();
	critChanceButton->Update();
}

void UpgradeUI::Render()
{
	if (!isActive)
		return;

	background->Render();
	healthButton->Render();
	Vector2 textPos = healthButton->GlobalPos() + Vector2(140, 0);
	Font::Get()->RenderText(healthButtonTag, { textPos.x, textPos.y }, { 220, 30 }, "Default", "White");

	physicAttackButton->Render();
	textPos = physicAttackButton->GlobalPos() + Vector2(140, 0);
	Font::Get()->RenderText(physicAttackButtonTag, { textPos.x, textPos.y }, { 220, 30 }, "Default", "White");

	magicAttackButton->Render();
	textPos = magicAttackButton->GlobalPos() + Vector2(140, 0);
	Font::Get()->RenderText(magicAttackButtonTag, { textPos.x, textPos.y }, { 220, 30 }, "Default", "White");

	critChanceButton->Render();
	textPos = critChanceButton->GlobalPos() + Vector2(140, 0);
	Font::Get()->RenderText(critChanceButtonTag, { textPos.x, textPos.y }, { 220, 30 }, "Default", "White");
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
