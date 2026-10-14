#pragma once

class Ninja : public GameObject // 최적화 대신 범용화
{
public:
	enum class ActionType // 상태
	{
		IDLE				= 0,
		RUN,
		JUMP,
		JUMP_UP,
		JUMP_DOWN,
		MELEE_ATTACK		= 10,
		JUMP_ATTACK,
	};

public:
	Ninja();
	~Ninja();

	void Update();
	void Render();
	void RenderUI();

	Collider* GetCollider() { return collider; }

	// 특수 상태 제어 함수 (매개변수 불필요한 이벤트용 직통)
	void SetIdle();
	void SetFall();
	bool IsJumping() { return curAction >= ActionType::JUMP && curAction <= ActionType::JUMP_DOWN; }

private:
	void ControlTemp(); // 임시 조작 함수 (나중에 컨트롤러로 변경)
	void Move();
	void Jump();
	void Attack();

	// 상태 제어 함수
	void SetAction(ActionType type); // 원형
	void SetActionCode(int type);        // 범용int버전


	void CreateActions(); // 액션 생성을 따로 분리

private:

	// 클립을 쓰지 않으므로 더욱더 게임 오브젝트이든 말든 상관없다
	// -> 오히려 나중에 전용 그래픽이 있거나 하면 있는 편이 나을 수 있다
	//    (예 : 스스로가 쿼드화)

	// 일단은 액션을 통해 자신의 그래픽을 표현한다

	map<ActionType, Action*> actions; // 액션의 모음집

	ActionType curAction = ActionType::IDLE;

	Texture* secondMap;

	IntValueBuffer* intValueBuffer;
	FloatValueBuffer* floatValueBuffer;

	Collider* collider;
};

