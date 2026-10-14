#include "Framework.h"
#include "CollisionScene.h"

#include "Objects/TestObject/TestQuad.h"
#include "Objects/TestObject/TestTarget.h"

CollisionScene::CollisionScene()
{
    testPlayer = new TestQuad();

	testTarget[0] = new TestTarget();
	testTarget[0]->Pos() = { CENTER_X * 0.5f, CENTER_Y };

	testTarget[1] = new TestTarget();
	testTarget[1]->Pos() = { CENTER_X * 1.5f, CENTER_Y };

	BulletManager::Get(); // 함수의 상수 호출화

	EffectManager::Get()->Add("Explosion1", 20,
		L"Textures/Effect/Explosion1.png", 4, 4, 1, false);

	EffectManager::Get()->Add("Explosion2", 20,
		L"Textures/Effect/Explosion2.png", 4, 2, 1, true);

	EffectManager::Get()->Add("Fire", 20,
		L"Textures/Effect/Fire.png", 3, 3); // 디폴트 인수 사용


	EffectManager::Get()->SetColor("Explosion1", 1, 0.2f, 0.2f);

}

CollisionScene::~CollisionScene()
{
    delete testPlayer;

	for (TestTarget* target : testTarget)
		delete target;

	BulletManager::Get()->Delete();
	EffectManager::Get()->Delete();
}

void CollisionScene::Update()
{
	if (KEY_DOWN('1'))
		EffectManager::Get()->Play("Explosion1", mousePos);

	if (KEY_DOWN('2'))
		EffectManager::Get()->Play("Explosion2", mousePos);

	if (KEY_DOWN('3'))
		EffectManager::Get()->Play("Fire", mousePos);

	for (TestTarget* target : testTarget)
	{
		// 각 (테스트) 충돌체가 플레이어에게 닿았는지 판별
		if (target->IsHit(testPlayer->GetCollider()))
		{
			// 맞으면 색이 바뀌도록 하자
			target->GetCollider()->GetColor() =
				{ 1, 0, 0, 1 }; // 빨간색
		}
		else // 안 맞으면
		{
			target->GetCollider()->GetColor() =
				{ 0, 1, 1, 1 }; // 청록색, 혹은 시안색
		}

		if (BulletManager::Get()->IsCollision(target->GetCollider()))
		{
			target->GetCollider()->GetColor() = { 1, 0, 0, 1 };
		}
		else { target->GetCollider()->GetColor() = { 0, 1, 1, 1 }; }

		// 충돌 판별 후 결과를 각각 업데이트
		target->Update();
	}

    testPlayer->Update();

	BulletManager::Get()->Update();
	EffectManager::Get()->Update();
}

void CollisionScene::Render()
{
	for (TestTarget* target : testTarget)
		target->Render();

    testPlayer->Render();

	BulletManager::Get()->Render();
	EffectManager::Get()->Render();
}

void CollisionScene::PostRender()
{
	testPlayer->RenderUI();
}
