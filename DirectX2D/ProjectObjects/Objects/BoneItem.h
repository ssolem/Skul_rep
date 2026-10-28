#pragma once
#include "DropItem.h"

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
