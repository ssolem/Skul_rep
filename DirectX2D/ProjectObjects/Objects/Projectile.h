#pragma once
class Animator;
class Character;
class Stage;

using OnHitEvent = function<void(Character*)>;

class Projectile : public GameObject
{
public:
	Projectile();
	~Projectile();

	virtual void Update();
	virtual void Render();

	void Init(const ProjectileData& data);	//종류 세팅 - 풀에서 꺼낼 때마다
	void Reset();							//상태 초기화 + 비활성화 = 풀 반환

	virtual void Fire(Vector2 pos, Vector2 dir, const HitData& hit, ProjectileTarget target, OnHitEvent onHit = nullptr, int maxHitCount = 0);
	virtual void Destroy();
	virtual void Hit(Character* character);

	void CheckTileCollision(Stage* stage);	//tileHitType에 따라 소멸 / 박힘 / 통과

	bool IsFlying() { return isFlying; }
	ProjectileTarget GetTarget() { return target; }
	Collider* GetCollider() { return collider; }

	void SetGroupId(unsigned int id) { groupId = id; }
	unsigned int GetGroupId() { return groupId; }
	bool CanHit() { return canHit; }
	void DisableHit() { canHit = false; }	//같은 그룹 총알이 이미 맞췄으면 판정만 끔

private:
	void UpdateRotation();
	Clip* FindHitClip();

private:
	Animator* animator;
	RectCollider* collider;

	ProjectileData data;
	HitData hitData;
	ProjectileTarget target = ProjectileTarget::Player;

	Vector2 velocity = Vector2::Zero;
	float lifeTime = 0.0f;
	bool isFlying = false;
	bool isDestroying = false;
	bool isStuck = false;				//Stick 타입이 타일에 닿아 멈춘 상태 (lifeTime까지 판정 유지)

	vector<Character*> alreadyHit;		//관통 투사체가 같은 대상을 매 프레임 때리지 않게
	OnHitEvent onHit = nullptr;			//둔화 같은 추가 효과

	unsigned int groupId = 0;	//0이면 그룹 없음 (개별 판정)
	bool canHit = true;

};
