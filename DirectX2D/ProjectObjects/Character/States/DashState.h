#pragma once
#include "ActionState.h"

class DashState : public ActionState
{
public:

	DashState(Character* character, ActionType type, const vector<string>& clipNames);
	~DashState() override;

	virtual void Update() override;
	virtual void UpdateCoolDown() override;
	virtual void Render() override;

	virtual void Start() override;
	virtual void End() override;

	void SetNextDirection(float dirX) { nextDirX = dirX; hasNextDir = true; }

	virtual bool IsReady() override;

private:

	float GetInputDirX() 
	{
		float x = 0.0f;
		if (KEY_PRESS(VK_RIGHT)) x += 1.0f;
		if (KEY_PRESS(VK_LEFT)) x -= 1.0f;

		return x;
	}

private:

	int dashCount = 0;
	int maxDashCount = 2;

	float dashCoolDown = 0.0f;
	float dashCoolDownMax = 0.4f;
	bool isDashing = false;

	Vector2 dashEffectPos;
	float dashEffectLocationOffset = 30.f;

	Vector2 dashDirection = Vector2::Zero;
	float nextDirX = 0.0f;
	bool hasNextDir = false;
	float dashSpeed = 800.0f;

	float dashDistance = 150.f;
	float dashMovedDistance = 0.0f;
};

