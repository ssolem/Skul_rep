#include "Framework.h"
#include "GoldItem.h"
#include "ProjectObjects/Character/Character.h"

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
	//TODO: 골드 시스템 생기면 player에게 amount 지급
}
