#include "Framework.h"
#include "BoneItem.h"
#include "ProjectObjects/Character/Player.h"

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
	if (Player* owner = (Player*)player)
		owner->AddBoneChip(amount);
}
