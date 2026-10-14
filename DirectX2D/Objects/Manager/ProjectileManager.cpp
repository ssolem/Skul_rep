#include "Framework.h"
#include "ProjectileManager.h"
#include "ProjectObjects/Objects/Projectile.h"
#include "ProjectObjects/Character/Character.h"

ProjectileManager::ProjectileManager()
{
	projectiles.resize(30);

	for (Projectile*& projectile : projectiles)
		projectile = new Projectile();
}

ProjectileManager::~ProjectileManager()
{
	for (Projectile* projectile : projectiles)
		delete projectile;
}


void ProjectileManager::Update()
{
	for (Projectile* projectile : projectiles)
		projectile->Update();
}

void ProjectileManager::Render()
{
	for (Projectile* projectile : projectiles)
		projectile->Render();
}

void ProjectileManager::Register(const string& key, const ProjectileData& data)
{
	if (HasData(key)) return;
	datas[key] = data;
}

bool ProjectileManager::HasData(const string& key)
{
	return datas.count(key) > 0;
}

void ProjectileManager::Fire(const string& key, Vector2 pos, Vector2 dir, const HitData& hit, ProjectileTarget target, unsigned int groupId)
{
	auto it = datas.find(key);
	if (it == datas.end()) return;  // 등록 안 된 키

	Projectile* projectile = GetAvailableProjectile();
	projectile->Init(it->second);	// 종류 세팅 (클립, 콜라이더)
	projectile->Fire(pos, dir, hit, target);
	projectile->SetGroupId(groupId);
}

//0은 "그룹 없음"이라 건너뜀 (unsigned라 오버플로우 시 0으로 돌아옴)

unsigned int ProjectileManager::NewGroupId()
{
	if (++groupCounter == 0) ++groupCounter;
	return groupCounter;
}

void ProjectileManager::CheckCollision(Character* character, ProjectileTarget side)
{
	for (Projectile* projectile : projectiles)
	{
		if (!projectile->IsFlying()) continue;
		if (!projectile->CanHit()) continue;
		if (projectile->GetTarget() != side) continue;
		if (!projectile->GetCollider()->IsCollision(character->GetCollider())) continue;

		projectile->Hit(character);

		//같은 그룹의 나머지 총알은 판정 끔

		unsigned int group = projectile->GetGroupId();
		if (group == 0) continue;

		for (Projectile* other : projectiles)
			if (other != projectile && other->GetGroupId() == group)
				other->DisableHit();
	}
}

Projectile* ProjectileManager::GetAvailableProjectile()
{
	for (Projectile* projectile : projectiles)
		if (!projectile->Active())
			return projectile;

	//풀 부족시
	Projectile* projectile = nullptr;
	for (int i = 0; i < 10; ++i)
	{
		projectile = new Projectile();
		projectiles.push_back(projectile);
	}
	return projectile;
}
