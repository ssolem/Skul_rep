#include "Framework.h"
#include "EnemyFactory.h"
#include "Normal/Ent.h"
#include "Normal/RootEnt.h"
#include "Normal/GiganticEnt.h"
#include "Normal/BlossomEnt.h"

//새 적을 추가하면 여기 if 한 줄 + include 한 줄
Enemy* EnemyFactory::Create(string type)
{
	if (type == "Ent")         return new Ent();
	if (type == "RootEnt")     return new RootEnt();
	if (type == "BlossomEnt")  return new BlossomEnt();
	if (type == "GiganticEnt") return new GiganticEnt();

	return nullptr;
}
