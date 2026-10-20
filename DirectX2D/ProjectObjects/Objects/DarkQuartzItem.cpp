#include "Framework.h"
#include "DarkQuartzItem.h"
#include "ProjectObjects/Character/Character.h"

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
	//TODO: 다크쿼츠 시스템 생기면 player에게 amount 지급
}
