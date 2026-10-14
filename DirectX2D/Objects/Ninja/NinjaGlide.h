#pragma once
#include "NinjaJump.h"

class NinjaGlide : public NinjaJump
{
public:
	NinjaGlide(Transform* target);
	~NinjaGlide() = default;

	virtual void Update() override;
};

