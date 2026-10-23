#pragma once

class Enemy;

class EnemyFactory
{
public:
	static Enemy* Create(string type);
	static bool IsValidType(string type);
	static vector<string> GetTypeNames();
};
