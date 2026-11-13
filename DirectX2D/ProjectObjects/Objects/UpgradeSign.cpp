#include "Framework.h"
#include "UpgradeSign.h"
#include "ProjectObjects/Objects/AnimObject.h"

UpgradeSign::UpgradeSign()
{
	position = new Transform();
	position->Scale() = SKUL_DEFAULT_ASSET_SIZE;

	sign = new AnimObject();
	sign->SetParent(position);
	sign->Scale() = Vector2::One;
	sign->AddClip("UpgradeSign", L"ProjectTextures/UpgradeSign", true);
	sign->Stop();

	collider = new RectCollider(COL_SIZE);
	collider->SetParent(position);
	collider->Pos() += COL_OFFSET;
	collider->SetActive(false);
}

UpgradeSign::~UpgradeSign()
{
	delete collider;
	delete sign;
	delete position;
}

void UpgradeSign::Update()
{
	sign->Update();
}

void UpgradeSign::Render()
{
	sign->Render();
	collider->Render();
}

void UpgradeSign::Show(Vector2 groundPos)
{
	position->Pos() = groundPos;
	position->UpdateWorld();

	sign->Play("UpgradeSign", Vector2());
	collider->UpdateWorld();
	collider->SetActive(true);
}

void UpgradeSign::Hide()
{
	sign->Stop();
	collider->SetActive(false);
}

bool UpgradeSign::IsInteractable(Collider* player)
{
	if (!collider->Active()) return false;

	return collider->IsCollision(player);
}
