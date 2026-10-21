#pragma once

class Stage;

struct DropTable
{
	//count - 개수 amount - 개당 얼마
	int goldCount = 0;		
	int goldAmount = 0;		

	int boneCount = 0;
	int boneAmount = 0;

	int quartzCount = 0;
	int quartzAmount = 0;
	float quartzChance = 1.0f;	//확률

	void Spawn(Stage* stage, Vector2 pos) const;
};
