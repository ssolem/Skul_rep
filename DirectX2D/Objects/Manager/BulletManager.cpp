#include "Framework.h"
#include "BulletManager.h"
#include "Objects/TestObject/TestBullet.h"
#include "ProjectileManager.h"

BulletManager::BulletManager()
{
	testBullets.resize(30); // 일단 크기 + 초기화(아직 널포인터)까지 싹 다 해둔다

	// -> 미리 만들어두면 게임에서 총알의 상한선 관리 + 로딩에 생기는 렉의 시점을 옮길 수 있다
	//    플레이 중의 최적화에 도움이 된다

	// -> 이런 방식을 오브젝트 풀링이라고 한다

	// 풀링의 완성 : 아예 인스턴스까지 다 만들어두기

	//for (TestBullet*& bullet : testBullets)
	//	bullet = new TestBullet();
}

BulletManager::~BulletManager()
{
	//for (TestBullet* bullet : testBullets)
	//	delete bullet;
}

void BulletManager::Update()
{
	//for (TestBullet* bullet : testBullets)
	//	bullet->Update();
}

void BulletManager::Render()
{
	//for (TestBullet* bullet : testBullets)
	//	bullet->Render();
}

void BulletManager::Fire(Vector2 pos, Vector2 direction, UINT bulletCode)
{
	// *(생략) 총알이 여러 가지인 경우 bulletCode를 사용해서 어느 총알에 생성 적용할지 봐야 한다
	//  -> swtich

	for (TestBullet* bullet : testBullets) // 일단 풀 검사 시작
	{
		if (!bullet->Active()) // 아직 잠자는 총알이 나오면
		{
			bullet->Fire(pos, direction); // 총알을 하나 발사시키고
			return;						  // 바로 종료
		}
	}
}

bool BulletManager::IsCollision(Collider* collider)
{
	for (TestBullet* bullet : testBullets)
	{
		if (bullet->Active() && bullet->GetCollider()->IsCollision(collider))
		{
			// 총알은 자체로 할 일을 이 안에서 따로 배정받는 것으로 하자

			return true;
		}
	}

	return false;
}
