#pragma once

class Stage;

class SkulObject : public Quad
{

	using LaunchMethod = function<void(SkulObject* object, float value)>;

public:
	SkulObject(wstring file);
	virtual ~SkulObject();

	void Update();
	void Render();
	void RenderUI();

	void Bounce(Vector2 normal, float decreaseRate = 0.6f, float randomRangeDeg = 0.0f);
	void KnockAway(SkulObject* other);

	void StartLaunch(LaunchMethod method);
	void ActivatePhysics();
	//벽/바닥/계단 타일과의 충돌을 스스로 처리 (head든 잔해든 공통으로 사용)
	//다른 오브젝트/적과의 상호작용은 던진 쪽(스킬 소유자)이 직접 검사
	void ResolveTileCollision(Stage* stage);

	Collider* GetCollider() { return collider; }
	Vector2 GetVelocity() { return velocity; }
	void SetVelocity(Vector2 velocity) { this->velocity = velocity; }
	void SetMass(float mass) { this->mass = mass; }
	void AddForce(Vector2 force) { this->force = force; }
	void SetGround(bool grounded) { isGrounded = grounded; }
	FlightState GetFlightState() { return flightState; }
	bool IsGrounded() { return isGrounded; }

	void SetCollider(Collider* collider) { this->collider = collider; }
	void SetInertia(float value) { this->momentOfInertia = value; }
	void SetDamping(float damp) { this->rotationDamping = damp; }

private:
	Vector2 GetRandomDirection(float centerAngle, float rangeDeg, float speed);

private:
	Collider* collider;

	bool isGrounded;
	float mass;
	float friction;

	float prevBottom = 0.0f;	//직전 프레임 콜라이더 바닥 높이 (위에서 착지했는지 판정용)

	FlightState flightState = FlightState::PHYSICS;
	LaunchMethod launchMethod;

	Vector2 velocity = Vector2::Zero;
	Vector2 gravity = Vector2(0.0f, GRAVITY);
	Vector2 force;
	Vector2 acceleration;
	Vector2 maxVelocity;

	float momentOfInertia = 50.0f;
	float angularVelocity = 0.0f;
	float rotationDamping = 6.0f;
	float visualAngle = 0.0f;	//렌더링에만 쓰는 회전값 - Rot().z(콜라이더 기준)는 건드리지 않는다
};

