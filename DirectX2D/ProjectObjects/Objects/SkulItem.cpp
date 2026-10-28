#include "Framework.h"
#include "SkulItem.h"
#include "ProjectObjects/Character/Player.h"
#include "ProjectObjects/Character/SkulClass.h"

SkulItem::SkulItem(SkulClass* skul)
{
	autoAcquire = false;
	SetSkul(skul);
}

SkulItem::~SkulItem()
{
	delete skul;	
}

void SkulItem::OnAcquire(Character* player)
{
	Player* owner = (Player*)player;

	SkulClass* dropped = owner->AcquireSkul(skul);
	skul = nullptr;

	if (dropped == nullptr) return;

	SetSkul(dropped);
	Spawn(stage, owner->GlobalPos());
}

void SkulItem::SetSkul(SkulClass* skul)
{
	this->skul = skul;
	SetImage(skul->GetDropImage());
}
