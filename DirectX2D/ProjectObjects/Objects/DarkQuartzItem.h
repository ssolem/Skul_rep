#pragma once
#include "DropItem.h"

class DarkQuartzItem : public DropItem
{
public:
	DarkQuartzItem(int amount);
	~DarkQuartzItem() override;

protected:
	virtual void OnAcquire(Character* player) override;

private:
	int amount;
};
