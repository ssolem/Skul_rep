#pragma once
#include "DropItem.h"

class GoldItem : public DropItem
{
public:
	GoldItem(int amount);
	~GoldItem() override;

protected:
	virtual void OnAcquire(Character* player) override;

private:
	int amount;
};
