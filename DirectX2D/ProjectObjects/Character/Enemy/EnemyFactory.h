#pragma once

class Enemy;

class EnemyFactory
{
public:
	static Enemy* Create(string type);
};
