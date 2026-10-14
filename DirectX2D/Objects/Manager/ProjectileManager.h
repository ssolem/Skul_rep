#pragma once

class Projectile;
class Character;

class ProjectileManager : public Singleton<ProjectileManager>
{
private:
	friend class Singleton;

	ProjectileManager();
	~ProjectileManager();

public:
	void Update();
	void Render();

	void Register(const string& key, const ProjectileData& data);
	bool HasData(const string& key);
	void Fire(const string& key, Vector2 pos, Vector2 dir, const HitData& hit, ProjectileTarget target, unsigned int groupId = 0);
	unsigned int NewGroupId();

	void CheckCollision(Character* character, ProjectileTarget side);
	
private:
	Projectile* GetAvailableProjectile();

private:
	map<string, ProjectileData> datas;
	vector<Projectile*> projectiles;
	unsigned int groupCounter = 0;

};

