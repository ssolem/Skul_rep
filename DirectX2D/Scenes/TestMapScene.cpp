#include "Framework.h"
#include "TestMapScene.h"
#include "Objects/Tile/GameTileMap.h"
#include "Objects/Ninja/Ninja.h"

TestMapScene::TestMapScene()
{
	map = new GameTileMap("TextData/scroll1.map", { 80, 80 });

	player = new Ninja();
	player->Pos() = { CENTER_X, 450 };
}

TestMapScene::~TestMapScene()
{
	delete player;
	delete map;
}

void TestMapScene::Update()
{
	RectCollider* playerCollider = (RectCollider*)(player->GetCollider());

	bool grounded = false;

    for (Tile* obj : map->GetObjTiles())
    {
        if (playerCollider->IsCollision(obj->GetCollider()) &&
            playerCollider->GetDirection(obj->GetCollider()) == GameMath::Direction::DOWN)
        {
            grounded = true;

            RectCollider* objCollider = (RectCollider*)obj->GetCollider();
            player->Pos().y += objCollider->T() - playerCollider->B();
            break;
        }
    }

    player->UpdateWorld();
    playerCollider->UpdateWorld();

    for (Tile* obj : map->GetObjTiles())
    {
        if (playerCollider->B() >= obj->GetCollider()->GlobalPos().y) continue;

        if (!playerCollider->IsCollision(obj->GetCollider())) continue;

        if (playerCollider->GlobalPos().x >= obj->GetCollider()->GlobalPos().x)
        {
            player->Pos().x += (obj->GetCollider()->R() - playerCollider->L());
            break;
        }
        else if (playerCollider->GlobalPos().x < obj->GetCollider()->GlobalPos().x)
        {
            player->Pos().x -= abs(playerCollider->R() - obj->GetCollider()->L());
            break;
        }
    }

	if (grounded && player->IsJumping())
		player->SetIdle();

	if (!grounded && !player->IsJumping())
		player->SetFall();

	player->Update();
}

void TestMapScene::Render()
{
	map->Render();
	player->Render();
}

void TestMapScene::PostRender()
{
	player->RenderUI();
}
