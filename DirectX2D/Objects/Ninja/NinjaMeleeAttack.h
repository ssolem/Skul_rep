#pragma once
#include "NinjaAttack.h"

class NinjaMeleeAttack : public NinjaAttack
{
public:
	NinjaMeleeAttack(Transform* target); // 표적* 자리는 지정
	~NinjaMeleeAttack() = default; // 부모에서 지정된 대로
};

