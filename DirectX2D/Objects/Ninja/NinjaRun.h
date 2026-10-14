#pragma once

class NinjaRun : public Action
{
public:
	NinjaRun(Transform* target);
	~NinjaRun() = default;

	virtual void Update() override;

	void SetVelocity(float velocity)
		{ this->velocity = velocity; }

private:

	Transform* target;
	float velocity;
	float MOVE_SPEED = 150;
};

