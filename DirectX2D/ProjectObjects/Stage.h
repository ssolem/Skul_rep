#pragma once

class SkulTileMap;
class SkulObject;
class Enemy;
class Character;
class IHittable;
class StageBackground;
class DropItem;
class WaveSpawner;

//맵 하나의 생명주기를 통째로 담당하는 클래스
//타일맵과 그 위에 떠 있는 스컬 오브젝트를 한 곳에서 관리
//Character/Skill 등 다른 클래스가 타일맵을 직접 들고 다니지 않고
//이 클래스를 통해서만 콜리전 체크를 하도록 한다.
//씬이 이 Stage를 소유하고, 씬이 바뀌면(delete stage) 타일맵/오브젝트가 함께 정리된다.
class Stage
{
public:
	Stage(string mapFile, Vector2 mapPos = { 0, 0 });
	~Stage();

	void Update();
	void Render();

	SkulTileMap* GetTileMap() { return tileMap; }

	//타일맵 콜리전 체크 (SkulTileMap에 위임, 다른 클래스는 tileMap을 직접 들고 있을 필요 없음)
	Tile* CheckFloorTileCollision(Collider* collider, Vector2* overlap = nullptr);
	Tile* CheckStairTileCollision(Collider* collider, Vector2* overlap = nullptr);
	Tile* CheckWallTileCollision(Collider* collider, Vector2* overlap = nullptr);

	//맵 위에 떠 있는 스컬 오브젝트 등록/해제
	//소유권은 만든 쪽(Skill 등)이 그대로 갖고, Stage는 충돌 조회용으로만 들고 있음
	void RegisterObject(SkulObject* obj);
	void UnregisterObject(SkulObject* obj);

	void RegisterEnemy(Enemy* enemy);
	void RemoveEnemy(Enemy* enemy);

	//mover를 제외한 다른 스컬 오브젝트와의 충돌 체크
	SkulObject* CheckOtherObjectCollision(SkulObject* mover, Vector2* overlap = nullptr);
	vector<SkulObject*> CheckAllOtherObjectCollision(SkulObject* mover);	//겹친 오브젝트 전부 (여러 개를 한 번씩 칠 때)
	Enemy* CheckEnemyCollision(Collider* collider, Vector2* overlap = nullptr);
	Enemy* FindClosestEnemy(Vector2 pos, float range);	//range 안의 살아있는 적 중 가장 가까운 적 (없으면 nullptr)
	int GetRemainEnemyCount();	//살아있는(활성) 적 수

	vector<IHittable*> CheckAllHittableCollision(Collider* collider, Vector2* overlap = nullptr);

	//DropItem
	void SpawnItem(DropItem* item, Vector2 pos);

	//웨이브 - StageData로 Build 후 Start
	WaveSpawner* GetWaveSpawner() { return waveSpawner; }

	void SetCameraBounds();
	void SetBackground(StageBackground* background) { this->background = background; }

	//적 AI가 추적할 대상 - 소유권은 Scene
	void SetPlayer(Character* player) { this->player = player; }
	Character* GetPlayer() { return player; }
	void SetBackgroundOffset(Vector2 offset) { backgroundOffset = offset; }
	Vector2 GetBackgroundPos();
	Vector2 GetGroundPos(Vector2 pos);

private:
	void CreateBoundWalls();	//맵 범위 바깥 좌우에 보이지 않는 벽

private:
	StageBackground* background = nullptr;
	SkulTileMap* tileMap = nullptr;
	Character* player = nullptr;
	vector<SkulObject*> objects;
	vector<Enemy*> enemies;
	vector<DropItem*> items;
	vector<Tile*> boundWalls;
	Vector2 backgroundOffset = Vector2::Zero;
	WaveSpawner* waveSpawner = nullptr;
};
