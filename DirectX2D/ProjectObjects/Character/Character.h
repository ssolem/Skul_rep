#pragma once
#include "../Interfaces/IDamageable.h"
#include "../Interfaces/IHittable.h"

class Animator;
class Skill;
class Stage;
class Stat;

class Character : public GameObject, public IHittable, public IDamageable
{
public:
	Character();
	virtual ~Character();

	void Init();

	virtual void Update();
	virtual void UpdateCollision();
	virtual void Render();
	virtual void PostRender();

	virtual void PlayClip(const string& name);
	virtual Clip* FindClip(const string& name);
	virtual void OnStartAction();
	virtual void OnEndAction();

	//같은 이동 상태라도 다음 프레임에 클립을 다시 재생 (스컬 태그가 바뀌어 클립을 교체해야 할 때)

	void RefreshMoveClip() { curMoveState = MoveState::NONE; }

	void CheckFacing();

	//interface
	virtual void OnHit(const HitData& hit) override;
	virtual void TakeDamage(int amount) override;

	//getset
	Vector2 GetDirection() const { return dir; }
	Collider* GetCollider() const { return collider; }
	Collider* GetAttackCollider() { return attackCollider; }
	bool FacingRight() const { return isFacingRight; }
	bool IsGrounded() const { return isGrounded; }
	Vector2 GetDefaultSize() const { return defaultSize; }
	bool IsPlayingAction() const { return isPlayingAction; }
	bool canCancel() const { return canCancelAction; }
	Stage* GetStage() { return stage; }
	Stat* GetStatComponent() { return stat; }
	int GetMaxHP() { return maxHP; }
	int GetCurHP() { return curHP; }

	//void SetMoveSpeed(float speed) { moveSpeed = speed; }
	void SetMaxHP();
	void SetCurHP(int value) { curHP = value; NotifyHPChanged(); }

	//체력 UI 연결용 - 체력이 바뀌는 모든 경로에서 호출됨 (콜백은 하나만 연결 가능)
	void SetOnHPChanged(function<void(float curHP, float maxHP)> callback) { onHPChanged = callback; }
	void SetJump(float height, float riseTime);	//최고점 높이 + 도달 시간으로 중력/낙하 상한 역산
	void SetCancellable(bool value) { canCancelAction = value; }
	void SetFacing(bool right);
	void SetVelocity(Vector2 value) { velocity = value; }
	void SetStage(Stage* stage) { this->stage = stage; }

	//virtual Skill* GetCurSkill() { return nullptr; }

protected:
	virtual void UpdateControl();   // 기본: dir 대로 Move()
	virtual void PostUpdate() {}

	virtual void InitBody();        // defaultSize + collider + groundSensor

	void Move();
	void Jump();
	void Fall(); //발판아래로 내려가는
	void KnockBack(float amount = 0.0f);
	virtual void StartKnockBack(const HitData& hit);
	virtual void EndKnockBack();

	virtual void Die();

	void NotifyHPChanged() { if (onHPChanged) onHPChanged((float)curHP, (float)maxHP); }

	void FloorTileCollision();
	void WallTileCollision();
	void StairTileCollision();

	bool IsPassingTile(float tileTopPos)
	{
		//하강중 + dropThrough->직전에 밟고있었던 타일을 담고 있음 
		// 다른 타일과 충돌하여도 같은 줄 임이 판정되면
		return isFalling && dropThrough != nullptr
			&& tileTopPos >= dropThrough->GetCollider()->B();
	}

	MoveState SenseMove();
	void SenseGrounded();
	void SetGrounded(Tile* floor);
	void PlayAnim();

	void MoveVector2(float x, float y);
	void ResetMove() { dir = Vector2::Zero; }

protected:
	const float DROP_TIME = 0.2f;	// 아래점프 통과 시간
	const float GROUND_IGNORE_TIME = 0.1f;
	const float LANDING_ALLOW_OFFSET = 6.0f; //콜라이더 파고듬 여유 판정 구간
	const float WALL_PUSH_OFFSET = 0.5f;

	Animator* animator = nullptr;

	Collider* collider = nullptr;
	Collider* groundSensor = nullptr;
	Collider* attackCollider = nullptr;

	Stat* stat;
	int maxHP;
	int curHP;
	function<void(float, float)> onHPChanged;

	Stage* stage = nullptr;
	Tile* groundFloor = nullptr;
	Tile* dropThrough = nullptr;
	bool isFalling = false;

	float jumpIgnoreGroundTime = 0.0f;		// 점프 직후 지면감지 무시 타이머
	float dropTimer = 0.0f;					// 아래점프 통과 지속 시간
	float prevBottom = 0.0f;				// 직전 프레임 발 높이 (편도 판정)

	MoveState curMoveState = MoveState::IDLE;

	bool isPlayingAction = false;
	bool canCancelAction = false;

	bool isGrounded = false;
	bool isFacingRight = true;
	bool prevFacing = true;
	
	//float moveSpeed = 200.0f;
	const float MAX_FALL_SPEED_RATIO = 1.6f;	// 낙하 상한 = 점프 초속 * 비율

	float jumpHeight = 190.0f;
	float jumpRiseTime = 0.4f;
	float gravity = GRAVITY;
	float maxFallSpeed = 0.0f;		// SetJump()에서 계산

	int jumpCount = 0;
	int maxJumpCount = 1000000;

	Vector2 velocity = Vector2::Zero;
	Vector2 dir = Vector2::Zero;

	Vector2 defaultSize;
	
	//KnockBack
	bool isKnockBack = false;
	float knockBackDuration = 0.25f;
	float knockBackDistance = 30.0f;
	float knockBackSpeed = 300.0f;
	float knockBackMoved = 0.0f;
	float knockBackDirX = 0.0f;
	bool isLaunched = false;		// 띄워진 넉백 - 착지할 때까지 넉백 유지
};
