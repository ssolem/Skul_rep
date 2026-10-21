#include "Framework.h"
#include "DropTable.h"
#include "ProjectObjects/Stage.h"
#include "GoldItem.h"
#include "BoneItem.h"
#include "DarkQuartzItem.h"

void DropTable::Spawn(Stage* stage, Vector2 pos) const
{
	if (stage == nullptr) return;

	//아이템 소유권은 Stage
	for (int i = 0; i < goldCount; i++)
		stage->SpawnItem(new GoldItem(goldAmount), pos);

	for (int i = 0; i < boneCount; i++)
		stage->SpawnItem(new BoneItem(boneAmount), pos);

	if (quartzCount > 0 && Random(0.0f, 1.0f) <= quartzChance)
	{
		for (int i = 0; i < quartzCount; i++)
			stage->SpawnItem(new DarkQuartzItem(quartzAmount), pos);
	}
}
