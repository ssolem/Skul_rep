#pragma once

class NinjaJump : public Action
{
public:
	NinjaJump(Transform* target);
	~NinjaJump() = default;

	virtual void Update() override;	// 일반 상태
	virtual void Start() override;  // 점프 직후 점프력 부여 상태

	void SetHorisontal(float speed) { horisontal = speed; }

protected:
// private:
	Transform* target; // <- 이제 전용된 데이터. 부모 트랜스폼을 의미

	// 점프 하면 필요한 데이터 1. 점프력 2. 중력 3. 그래서 지금 속력
	
	float JUMP_POWER = 500; // 점프력
	float velocity = 0;	    // 현재 속력 (Y축)
							// * 중력은 전역 (월드) 데이터

	float horisontal = 0;   // 수평 이동 속력
	float MOVE_SPEED = 100; // 지상, 공중 속력이 다른 경우에는 이렇게
};

