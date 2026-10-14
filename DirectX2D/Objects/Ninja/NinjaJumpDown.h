#pragma once
#include "NinjaJump.h"

class NinjaJumpDown : public NinjaJump
{
public:

	NinjaJumpDown(Transform* target);
	~NinjaJumpDown() = default;

	virtual void Update() override;
	virtual void Start() override;
};

