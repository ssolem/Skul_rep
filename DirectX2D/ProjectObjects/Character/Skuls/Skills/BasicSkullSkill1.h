#pragma once
#include "../../Skill.h"

class BasicSkul;
class SkulObject;
class IHittable;

class BasicSkullSkill1 : public Skill
{
public:
	BasicSkullSkill1(Character* owner, float coolDown, BasicSkul* basicSkul);
	~BasicSkullSkill1() override;

	virtual void Init() override;

	virtual void Start() override;
	virtual void End() override;
	virtual bool IsReady() override;
	virtual void SetIcon() override;

	//충돌 검사는 BasicSkul이 하고, 데미지 계산/타격은 스킬이 담당
	void OnHeadHit(IHittable* target);	//기본 공격과 같이 IHittable로 타격 (Enemy의 OnHit은 protected)
	void OnHeadHitObject(SkulObject* object);	//다른 스컬 오브젝트를 날려 보냄 (던질 때마다 오브젝트당 한 번)

private:
	SkulObject* head = nullptr;
	vector<SkulObject*> alreadyHit;	//이번 던지기에서 이미 날린 오브젝트 - Start()에서 비움
	BasicSkul* basicSkul = nullptr;	//던질 머리(head)는 basicSkul이 소유 - 2번 스킬과 공유하기 위함

	float headSpeed = 500.0f;
	bool rightDirection = true;

	Vector2 throwPosOffset = Vector2::Zero;
	float throwRange = 1000.0f;
};
