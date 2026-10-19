#include "Framework.h"
#include "Stage.h"
#include "ProjectObjects/Tile/SkulTileMap.h"
#include "ProjectObjects/Objects/SkulObject.h"
#include "ProjectObjects/Character/Enemy/Normal/Enemy.h"
#include "ProjectObjects/StageBackground.h"

Stage::Stage(string mapFile, Vector2 mapPos)
{
	tileMap = new SkulTileMap(mapFile, mapPos);
}

Stage::~Stage()
{
	delete tileMap;

	for (Enemy* enemy : enemies)
		delete enemy;
}

void Stage::Update()
{
	if (background) background->Update();

	//등록된 스컬 오브젝트 전부 액션 상태와 무관하게 매 프레임 갱신
	for (SkulObject* obj : objects)
	{
		if (!obj->Active()) continue;

		obj->Update();
		obj->ResolveTileCollision(this);
	}

	for (Enemy* enemy : enemies)
	{
		if (!enemy->Active()) continue;

		enemy->Update();
	}

	ProjectileManager::Get()->CheckCollision(player, ProjectileTarget::Player);
	for (Enemy* enemy : enemies)
	{
		if (!enemy->Active()) continue;
		ProjectileManager::Get()->CheckCollision(enemy, ProjectileTarget::Enemy);
	}

	ProjectileManager::Get()->Update();
	ProjectileManager::Get()->CheckTileCollision(this);	//이동 후 타일에 닿았는지 확인
}

void Stage::Render()
{
	if (background) background->Render();

	tileMap->Render();

	for (SkulObject* obj : objects)
		if (obj->Active()) obj->Render();

	for (Enemy* enemy : enemies)
		if (enemy->Active()) enemy->Render();

	ProjectileManager::Get()->Render();
}

Tile* Stage::CheckFloorTileCollision(Collider* collider, Vector2* overlap)
{
	return tileMap->CheckFloorTileCollision(collider, overlap);
}

Tile* Stage::CheckStairTileCollision(Collider* collider, Vector2* overlap)
{
	return tileMap->CheckStairTileCollision(collider, overlap);
}

Tile* Stage::CheckWallTileCollision(Collider* collider, Vector2* overlap)
{
	return tileMap->CheckWallTileCollision(collider, overlap);
}

void Stage::RegisterObject(SkulObject* obj)
{
	objects.push_back(obj);
}

void Stage::UnregisterObject(SkulObject* obj)
{
	vector<SkulObject*>::iterator it = find(objects.begin(), objects.end(), obj);
	if (it != objects.end())
		objects.erase(it);
}

void Stage::RegisterEnemy(Enemy* enemy)
{
	enemies.push_back(enemy);
	enemy->SetStage(this);
}

void Stage::RemoveEnemy(Enemy* enemy)
{
	auto it = find(enemies.begin(), enemies.end(), enemy);
	if (it != enemies.end())
	{
		enemies.erase(it);
		delete enemy;
	}
}

SkulObject* Stage::CheckOtherObjectCollision(SkulObject* mover, Vector2* overlap)
{
	for (SkulObject* other : objects)
	{
		if (other == mover) continue;
		if (!other->Active()) continue;	//비활성 오브젝트는 충돌 대상에서 제외

		if (other->GetCollider()->IsCollision(mover->GetCollider(), overlap))
			return other;
	}

	return nullptr;
}

vector<SkulObject*> Stage::CheckAllOtherObjectCollision(SkulObject* mover)
{
	vector<SkulObject*> hits;

	for (SkulObject* other : objects)
	{
		if (other == mover) continue;
		if (!other->Active()) continue;	//비활성 오브젝트는 충돌 대상에서 제외

		if (other->GetCollider()->IsCollision(mover->GetCollider()))
			hits.push_back(other);
	}

	return hits;
}

Enemy* Stage::CheckEnemyCollision(Collider* collider, Vector2* overlap)
{
	for (Enemy* enemy : enemies)
	{
		if (!enemy->Active()) continue;	//죽은(비활성) 적은 충돌 대상에서 제외

		if (enemy->GetCollider()->IsCollision(collider, overlap))
			return enemy;
	}

	return nullptr;
}

Enemy* Stage::FindClosestEnemy(Vector2 pos, float range)
{
	Enemy* closest = nullptr;
	float closestDist = range;

	for (Enemy* enemy : enemies)
	{
		if (!enemy->Active()) continue;

		float dist = (enemy->GlobalPos() - pos).Length();
		if (dist > closestDist) continue;

		closestDist = dist;
		closest = enemy;
	}

	return closest;
}

vector<IHittable*> Stage::CheckAllHittableCollision(Collider* collider, Vector2* overlap)
{
	vector<IHittable*> hits;

	for (Enemy* enemy : enemies)
	{
		if (!enemy->Active()) continue;	//죽은(비활성) 적은 맞지 않음

		if (enemy->GetCollider()->IsCollision(collider, overlap))
			hits.push_back(enemy);
	}

	for (SkulObject* obj : objects)
	{
		if (!obj->Active()) continue;	//비활성 오브젝트는 맞지 않음

		//IHittable을 상속받지 않는 오브젝트의 경우 사정하여
		IHittable* hittable = dynamic_cast<IHittable*>(obj);
		if (hittable == nullptr) continue;

		if (obj->GetCollider()->IsCollision(collider))
			hits.push_back(hittable);
	}

	return hits;
}

void Stage::SetCameraBounds()
{
	Vector2 leftBottom = tileMap->GetLeftBottom();
	Vector2 rightTop = tileMap->GetRightTop();

	Vector2 mapSize = rightTop - leftBottom;
	Vector2 camSize = CAM->deviceSize;

	CAM->SetLeftBottom(leftBottom.x, leftBottom.y);
	CAM->SetRightTop(rightTop.x, rightTop.y);
}

Vector2 Stage::GetBackgroundPos()
{
	return tileMap->GetMidPos() + backgroundOffset;
}

Vector2 Stage::GetGroundPos(Vector2 pos)
{
	Vector2 gridIndex = tileMap->WorldToGridPos(pos);
	Vector2 minIndex = tileMap->GetMinIndex();
	const map<Vector2, Tile*, Vector2IntLess>& floorTiles = tileMap->GetFloorTiles();
	const map<Vector2, Tile*, Vector2IntLess>& stairTiles = tileMap->GetStairTiles();
	
	int x = gridIndex.x;
	for (int i = gridIndex.y; i >= minIndex.y; --i)
	{
		auto it = floorTiles.find(Vector2(x, i));
		if (it != floorTiles.end())
		{
			return Vector2(pos.x, it->second->GetCollider()->T());
		}

		auto staitIt = stairTiles.find(Vector2(x, i));
		if (staitIt != stairTiles.end())
		{
			return Vector2(pos.x, staitIt->second->GetCollider()->T());
		}
	}

	return pos;
}


