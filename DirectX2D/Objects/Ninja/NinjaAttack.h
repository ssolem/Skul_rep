#pragma once
class NinjaAttack : public Action
{
public:
	NinjaAttack(string file, Transform* target); // file : 종류, target : 표적 혹은 적중대상
	~NinjaAttack();

	virtual void Render() override;
	virtual void End() override;

	virtual void EnableCollider();	// 판정의 활성화
	virtual void DisableCollider(); // 판정의 종료

protected:

	Transform* target;
	float power = 0;

	Collider* attackCollider;

};

