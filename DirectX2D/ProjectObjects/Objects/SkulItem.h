#pragma once
#include "DropItem.h"

class SkulClass;

class SkulItem : public DropItem
{
public:
	SkulItem(SkulClass* skul);
	~SkulItem() override;

protected:
	virtual void OnAcquire(Character* player) override;

private:
	void SetSkul(SkulClass* skul);

private:
	SkulClass* skul = nullptr;
};
