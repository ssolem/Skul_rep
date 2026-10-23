#pragma once
#include "../SkulClass.h"

class SkulObject;
class Stage;
class BasicSkullSkill1;

class BasicSkul : public SkulClass
{
public:
	BasicSkul();
	virtual ~BasicSkul() override;

	virtual void Update() override;
	virtual void Render() override;

	virtual void Init(Character* owner, Animator* animator) override;
	virtual void CreateAnimations(Animator* animator) override;
	virtual void SetAnimations() override;
	virtual void CreateEffects(Animator* animator) override;
	virtual void SetEffectDetails() override;
	virtual void SetStats() override;
	virtual void SetImages() override;


	//머리(SkulObject)는 1번 스킬(던지기)과 2번 스킬(머리 위치로 이동)이 공유해야 해서 BasicSkul이 소유한다
	SkulObject* GetHead() { return head; }
	void RegisterHeadToStage(Stage* stage);
	void RetrieveHead();	//머리 회수 (직접 닿았을 때, 2번 스킬 사용 시 공통)

	virtual const vector<AttackData>& GetComboAttacks() const override { return basicAttackCombos; }

private:

	virtual void CreateSkills() override;
	void CreateHead();
	virtual void CreateComboAttacks(Animator* animator) override;

private:
	SkulObject* head = nullptr;
	BasicSkullSkill1* throwSkill = nullptr;	//머리가 적에 맞았을 때 데미지 처리를 맡길 1번 스킬
	bool canRetrieveHead = false;	//던진 직후엔 캐릭터와 겹쳐 있으므로, 한 번 떨어진 뒤부터 닿으면 회수
};

