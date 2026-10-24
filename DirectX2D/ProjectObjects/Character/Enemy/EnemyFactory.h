#pragma once

class Enemy;

class EnemyFactory
{
public:
	EnemyFactory() {}
	~EnemyFactory() {}

	static Enemy* Create(string type);
	static bool IsValidType(string type);
	static vector<string> GetTypeNames();
};
