#include "Framework.h"
#include "RewardDoors.h"
#include "ProjectObjects/Objects/AnimObject.h"

RewardDoors::RewardDoors()
{
	Init();
}

RewardDoors::~RewardDoors()
{
	delete gateTable;
	delete leftDoor;
	delete rightDoor;
	delete gateWall;
}

void RewardDoors::Update() 
{
    gateWall->UpdateWorld();
    gateTable->UpdateWorld();

    leftDoor->Update(); 
    rightDoor->Update();
}

void RewardDoors::Render() 
{
    gateWall->Render();
    leftDoor->Render();
    rightDoor->Render();
    gateTable->Render();
}

void RewardDoors::Open()
{        
    if (leftReward != RewardType::None)
        leftDoor->Play(GetGateTag(leftReward) + "Activate", leftDoor->Pos());

    if (rightReward != RewardType::None)
        rightDoor->Play(GetGateTag(rightReward) + "Activate", rightDoor->Pos());
}

void RewardDoors::SetGroundPos(Vector2 groundPos)
{
	//벽 Quad는 중앙 기준이라 바닥 위치에서 반 높이만큼 올림
	float halfHeight = gateWall->GetSize().y * gateWall->Scale().y * 0.5f;
	gateWall->Pos() = groundPos + Vector2(0.0f, halfHeight);
}

void RewardDoors::Init()
{
    gateWall = new Quad(L"ProjectTextures/Gate/Chapter1/RewardStage/Ch1_Gate_Wall.png");
    gateTable = new Quad(L"ProjectTextures/Gate/Chapter1/RewardStage/Ch1_Gate_Table.png");

    gateTable->SetParent(gateWall);
    gateTable->Pos() += TABLE_POS;

    gateWall->Scale() = SKUL_DEFAULT_ASSET_SIZE;  

    leftDoor = new AnimObject();
    rightDoor = new AnimObject();
    CreateDoorAnim(leftDoor);
    CreateDoorAnim(rightDoor);

    SetDoorType();
}

void RewardDoors::CreateDoorAnim(AnimObject* door)
{
    door->SetParent(gateWall);
    door->Scale() = Vector2(1.0f, 1.0f);    

    door->AddClip("Activate", L"ProjectTextures/Gate/Chapter1/Normal", true, 1.0f, "GateNormal");
    door->AddClip("Deactivate", L"ProjectTextures/Gate/Chapter1/Normal", false, 1.0f, "GateNormal");

    door->AddClip("Activate", L"ProjectTextures/Gate/Chapter1/Chest", true, 1.0f, "GateChest");
    door->AddClip("Deactivate", L"ProjectTextures/Gate/Chapter1/Chest", false, 1.0f, "GateChest");

    door->AddClip("Activate", L"ProjectTextures/Gate/Chapter1/Grave", true, 1.0f, "GateGrave");
    door->AddClip("Deactivate", L"ProjectTextures/Gate/Chapter1/Grave", false, 1.0f, "GateGrave");

    door->AddClip("Deactivate", L"ProjectTextures/Gate/Chapter1/Terminal_Destroyed", false, 1.0f, "GateDestroyed");
}

void RewardDoors::SetDoorType()
{
    //{ None, Gold, Skul, Item } 중에서 중복 없이 2개
    int count = (int)RewardType::Max;
    int first = Random(0, count);
    int second = Random(0, count - 1);
    if (second >= first) second++; 

    leftReward = (RewardType)first;
    rightReward = (RewardType)second;

    PlayClosed(leftDoor, leftReward, LEFT_DOOR_POS);
    PlayClosed(rightDoor, rightReward, RIGHT_DOOR_POS);
}

void RewardDoors::PlayClosed(AnimObject* door, RewardType reward, Vector2 pos)
{
    door->Play(GetGateTag(reward) + "Deactivate", pos);
}

string RewardDoors::GetGateTag(RewardType reward)
{
    switch (reward)
    {
    case RewardType::Gold: return "GateNormal";
    case RewardType::Skul: return "GateGrave";
    case RewardType::Item: return "GateChest";
    }

    return "GateDestroyed";
}
