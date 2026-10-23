#include "Framework.h"
#include "Stage.h"
#include "ProjectObjects/Tile/SkulTileMap.h"
#include "ProjectObjects/Objects/SkulObject.h"
#include "ProjectObjects/Character/Enemy/Normal/Enemy.h"
#include "ProjectObjects/StageBackground.h"
#include "ProjectObjects/Objects/DropItem.h"
#include "ProjectObjects/WaveSpawner.h"

Stage::Stage(string mapFile, Vector2 mapPos)
{
	tileMap = new SkulTileMap(mapFile, mapPos);
	CreateBoundWalls();

	waveSpawner = new WaveSpawner(this);
}

Stage::~Stage()
{
	delete waveSpawner;	//적 포인터만 들고 있음 - 적 삭제는 아래 enemies에서
	delete tileMap;

	for (Enemy* enemy : enemies)
		delete enemy;

	for (DropItem* item : items)
		delete item;

	for (Tile* wall : boundWalls)
		delete wall;
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

	waveSpawner->Update();	//이번 프레임에 나올 적 Spawn + 웨이브 전환 판정

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

	for (DropItem* item : items)
		item->Update();

	//획득되어 꺼진 아이템 정리
	for (auto it = items.begin(); it != items.end();)
	{
		if ((*it)->Active()) { ++it; continue; }

		delete *it;
		it = items.erase(it);
	}
}

void Stage::Render()
{
	if (background) background->Render();

	tileMap->Render();

	for (Tile* wall : boundWalls)
		wall->GetCollider()->Render();	

	for (SkulObject* obj : objects)
		if (obj->Active()) obj->Render();

	for (Enemy* enemy : enemies)
		if (enemy->Active()) enemy->Render();

	for (DropItem* item : items)
		item->Render();

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
	if (Tile* wall = tileMap->CheckWallTileCollision(collider, overlap))
		return wall;

	for (Tile* wall : boundWalls)
		if (wall->GetCollider()->IsCollision(collider, overlap))
			return wall;

	return nullptr;
}

void Stage::CreateBoundWalls()
{
	Vector2 leftBottom = tileMap->GetLeftBottom();
	Vector2 rightTop = tileMap->GetRightTop();

	float thickness = 200.0f;		
	float extraTop = WIN_HEIGHT;	
	float height = (rightTop.y - leftBottom.y) + extraTop;
	float centerY = leftBottom.y + height * 0.5f;

	//맵 범위 바로 바깥에 붙여서 안쪽 면 = 맵 끝
	float xs[2] = { leftBottom.x - thickness * 0.5f, rightTop.x + thickness * 0.5f };
	for (float x : xs)
	{
		Tile* wall = new Tile(Vector2(thickness, height), Tile::Type::WALL);
		wall->Pos() = Vector2(x, centerY);
		wall->UpdateWorld();
		wall->GetCollider()->UpdateWorld();
		boundWalls.push_back(wall);
	}
}

void Stage::RegisterObject(SkulObject* obj)
{
	//이미 등록돼 있으면 무시
	if (find(objects.begin(), objects.end(), obj) != objects.end())
		return;

	objects.push_back(obj);
}

void Stage::UnregisterObject(SkulObject* obj)
{
	vector<SkulObject*>::iterator it = find(objects.begin(), objects.end(), obj);
	if (it != objects.end())
		objects.erase(it);
}

void Stage::SpawnItem(DropItem* item, Vector2 pos)
{
	item->Spawn(this, pos);
	items.push_back(item);
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

int Stage::GetRemainEnemyCount()
{
	int count = 0;
	for (Enemy* enemy : enemies)
		if (enemy->IsRemaining()) count++;	//등장 이펙트 중인 적도 포함

	return count;
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


