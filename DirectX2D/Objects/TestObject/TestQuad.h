#pragma once

class TestQuad : public Quad
{
public:

	TestQuad();
	~TestQuad();

	void Update();
	void Render() override;
	void PostRender();

	Collider* GetCollider() { return collider; }

	int GetAmmo() { return curAmmo; }
	int GetMaxAmmo() { return maxAmmo; }

	void RefillAmmo() { curAmmo = maxAmmo;	mags->SetAmount(1.0f); }

private:

	void SetController();

	void Rotate();
	void Move();

	void FollowMouseCursor();

	void RotateCW(void* value);
	void RotateCCW(void* value);

	void MoveUp(void* value);
	void MoveDown(void* value);
	void MoveLeft(void* value);
	void MoveRight(void* value);

	void MouseMove(void* value);
	bool mouseMoveMode = false;
	const bool bFALSE = false;
	const bool bTRUE = true;

	////////////////////////////////////////
	// Vector2 입력을 진행할 때

	void MoveVector2(void* value); // 입력을 받을 때 실행해줄 함수
	void MoveVector2(float x, float y); // 성분을 받아서 벡터를 구성해줄 함수

	Vector2 moveValue;

	////////////////////////////////////////

	void Fire(void* value); // <- 이제는 쓸모 없지만, 조작 인터페이스로서

	ObjectController* controller;
	Collider* collider;

	void ResetDirection() { dir = { 0, 0 }; }

	Vector2 dir;
	float rotation;

	float speedMult = 200;
	float rotationMult = 10;

	// 속성 : 그래픽으로 무장 부여
	Quad* turret;

	//테스트용 체력바 -> 탄창처럼 써보기
	ProgressBar* mags;
	int curAmmo;
	int maxAmmo;

};

