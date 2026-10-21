#include "Framework.h"
#include "GoldItem.h"
#include "ProjectObjects/Character/Player.h"

GoldItem::GoldItem(int amount)
	: amount(amount)
{
	SetIdleClip(L"ProjectTextures/Common/Gold/Gold_Spin.png");
	SetAcquireEffect("GoldGet", L"ProjectTextures/Common/Gold/Gold_Get.png");
}

GoldItem::~GoldItem()
{
}

void GoldItem::OnAcquire(Character* player)
{
	if (Player* owner = (Player*)player)
		owner->AddGold(amount);
}
