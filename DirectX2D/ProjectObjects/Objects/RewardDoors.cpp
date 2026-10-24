#include "Framework.h"
#include "RewardDoors.h"
#include "ProjectObjects/Character/Animator.h"
#include "ProjectObjects/Objects/AnimObject.h"

RewardDoors::RewardDoors()
{
	Init();
}

RewardDoors::~RewardDoors()
{
	delete gateChoiceTable;
	delete gateTable;
	delete leftDoor;
	delete rightDoor;
	delete gateWall;
}

void RewardDoors::Update() {}
void RewardDoors::Render() {}

void RewardDoors::Init()
{
    gateWall = new Quad(L"ProjectTextures/Gate/Chapter1/RewardStage/Ch1_Gate_Wall.png");
    gateTable = new Quad(L"ProjectTextures/Gate/Chapter1/RewardStage/Ch1_Gate_Table.png");
    gateChoiceTable = new Quad(L"ProjectTextures/Gate/Chapter1/RewardStage/Ch1_Gate_ChoiceTable.png");

    gateTable->SetParent(gateWall);
    gateChoiceTable->SetParent(gateWall);

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
	//랜덤으로 미중복으로 RewardType { None, Gold, Skul, Item } 중에서 2개 뽑기
	//순서대로 left, right에 주고  Deactivate로, None이면 Terminal_Destroyed 
}
