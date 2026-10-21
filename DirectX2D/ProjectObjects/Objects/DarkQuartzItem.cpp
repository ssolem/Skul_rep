#include "Framework.h"
#include "DarkQuartzItem.h"
#include "ProjectObjects/Character/Player.h"

DarkQuartzItem::DarkQuartzItem(int amount)
	: amount(amount)
{
	SetImages(L"ProjectTextures/Common/DarkQuartz/DarkQuartz.png");
	SetAcquireEffect("DarkQuartzGet", L"ProjectTextures/Common/DarkQuartz/DarkQuartz_Get.png");
}

DarkQuartzItem::~DarkQuartzItem()
{
}

void DarkQuartzItem::OnAcquire(Character* player)
{
	if (Player* owner = (Player*)player)
		owner->AddDarkQuartz(amount);
}
