#include "Framework.h"
#include "BoneItem.h"
#include "ProjectObjects/Character/Character.h"

BoneItem::BoneItem(int amount)
	: amount(amount)
{
	SetImages(L"ProjectTextures/Common/Bone/BoneChip.png");
	SetAcquireEffect("BoneGet", L"ProjectTextures/Common/Bone/Bones_Get.png");
}

BoneItem::~BoneItem()
{
}

void BoneItem::OnAcquire(Character* player)
{
	//TODO: 뼈 조각 시스템 생기면 player에게 amount 지급
}
