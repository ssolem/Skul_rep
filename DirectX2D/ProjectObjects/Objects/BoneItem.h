#pragma once
#include "DropItem.h"

//뼈 조각 - 애니메이션 없이 BoneChip_1~4 중 하나
class BoneItem : public DropItem
{
public:
	BoneItem(int amount);
	~BoneItem() override;

protected:
	virtual void OnAcquire(Character* player) override;

private:
	int amount;
};
