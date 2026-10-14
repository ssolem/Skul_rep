#pragma once

// 총알 종류라면 다 가져와서 관리해주는 전역 클래스 (싱글턴)

class TestBullet;

class BulletManager : public Singleton<BulletManager> // 템플릿 싱글턴
{
private:
	//싱글턴으로서 가지는 모든 기능을 쓰기 위해서
	friend class Singleton; // 부모 클래스와 결연

	BulletManager();
	~BulletManager();

public:
	void Update();
	void Render();

	// 총알을 발사하기 위한 함수
	void Fire(Vector2 pos, Vector2 direction, UINT bulletCode);

	// 충돌 관련 부가 함수
	bool IsCollision(Collider* collider);

private:
	vector<TestBullet*> testBullets; // 복수 총알 대비

};

