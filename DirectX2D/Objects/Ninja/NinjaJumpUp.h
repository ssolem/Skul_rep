#pragma once
#include "NinjaJump.h"

class NinjaJumpUp : public NinjaJump
{
public:

	NinjaJumpUp(Transform* target);
	~NinjaJumpUp() = default;

	virtual void Update() override;
	virtual void Start() override;

private:


};

