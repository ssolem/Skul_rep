#pragma once
#include "DropItem.h"

//다크쿼츠 - 애니메이션 없이 DarkQuartz_1~4 중 하나
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
