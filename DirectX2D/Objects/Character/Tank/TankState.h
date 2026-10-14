#pragma once
class TankState // NPC 행동에서 Task로 지칭되는 상태의 행동
{
public:

	TankState(Transform* tank);
	~TankState();

	virtual void Update();
	virtual void Render();
	virtual void SetDestPos(Vector2 pos);
	virtual void Start();

protected:
	float speed = 200.0f;

	Transform* tank = nullptr;	//이 행동의 주체
	Vector2 destPos; //목적지
};

