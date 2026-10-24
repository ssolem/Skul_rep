#include "Framework.h"
#include "ExitDoors.h"
#include "ProjectObjects/Objects/AnimObject.h"

ExitDoors::ExitDoors()
{
	root = new Transform();
	root->Scale() = SKUL_DEFAULT_ASSET_SIZE;

	gateWall = new Quad(L"ProjectTextures/Gate/Chapter1/RewardStage/Ch1_Gate_Wall.png");
	gateWall->SetParent(root);
	gateWall->Pos() = Vector2(0.0f, gateWall->GetSize().y * 0.5f);	

	gateTable = new Quad(L"ProjectTextures/Gate/Chapter1/RewardStage/Ch1_Gate_Table.png");
	gateTable->SetParent(gateWall);
	gateTable->Pos() = TABLE_POS;

	for (int i = 0; i < MAX_DOORS; i++)
	{
		doorObjects[i] = new AnimObject();
		CreateDoorAnim(doorObjects[i]);
	}

	Setup(RandomRewardDoors(), true);
}

ExitDoors::~ExitDoors()
{
	for (AnimObject* door : doorObjects)
		delete door;

	delete gateTable;
	delete gateWall;
	delete root;
}

void ExitDoors::Update()
{
	root->UpdateWorld();
	gateWall->UpdateWorld();
	gateTable->UpdateWorld();

	for (AnimObject* door : doorObjects)
		door->Update();	
}

void ExitDoors::Render()
{
	if (withFrame) gateWall->Render();

	for (AnimObject* door : doorObjects)
		door->Render();

	if (withFrame) gateTable->Render();
}

void ExitDoors::Setup(const vector<DoorInfo>& doors, bool withFrame)
{
	this->withFrame = withFrame;

	infos = doors;
	if (infos.size() > MAX_DOORS)
		infos.resize(MAX_DOORS);

	int count = (int)infos.size();
	for (int i = 0; i < MAX_DOORS; i++)
	{
		if (i >= count)
		{
			doorObjects[i]->Stop();	
			continue;
		}

		float side = (count == 1) ? 0.0f : (i == 0 ? -1.0f : 1.0f);

		if (withFrame)
			doorObjects[i]->Pos() = gateWall->Pos() + Vector2(FRAME_DOOR_OFFSET.x * side, FRAME_DOOR_OFFSET.y);
		else
			doorObjects[i]->Pos() = Vector2(DOOR_GAP * side, 0.0f);

		PlayDoor(i, false);	
	}
}

void ExitDoors::Open()
{
	for (int i = 0; i < (int)infos.size(); i++)
	{
		if (!infos[i].destroyed)
			PlayDoor(i, true);
	}
}

vector<DoorInfo> ExitDoors::RandomRewardDoors()
{
	int count = (int)RewardType::Max;
	int first = Random(0, count);
	int second = Random(0, count - 1);
	if (second >= first) second++;	

	vector<DoorInfo> doors(2);
	doors[0].destroyed = (first == 0);
	doors[0].reward = (RewardType)first;	
	doors[1].destroyed = (second == 0);
	doors[1].reward = (RewardType)second;

	return doors;
}

void ExitDoors::CreateDoorAnim(AnimObject* door)
{
	door->SetParent(root);
	door->Scale() = Vector2(1.0f, 1.0f);	

	door->AddClip("Activate", L"ProjectTextures/Gate/Chapter1/Normal", true, 1.0f, "GateNormal");
	door->AddClip("Deactivate", L"ProjectTextures/Gate/Chapter1/Normal", false, 1.0f, "GateNormal");

	door->AddClip("Activate", L"ProjectTextures/Gate/Chapter1/Chest", true, 1.0f, "GateChest");
	door->AddClip("Deactivate", L"ProjectTextures/Gate/Chapter1/Chest", false, 1.0f, "GateChest");

	door->AddClip("Activate", L"ProjectTextures/Gate/Chapter1/Grave", true, 1.0f, "GateGrave");
	door->AddClip("Deactivate", L"ProjectTextures/Gate/Chapter1/Grave", false, 1.0f, "GateGrave");

	door->AddClip("Deactivate", L"ProjectTextures/Gate/Chapter1/Terminal_Destroyed", false, 1.0f, "GateDestroyed");

	door->AddClip("Activate", L"ProjectTextures/Gate/Chapter1/Elite", true, 1.0f, "GateElite");
	door->AddClip("Deactivate", L"ProjectTextures/Gate/Chapter1/Elite", false, 1.0f, "GateElite");

	door->AddClip("Activate", L"ProjectTextures/Gate/Chapter1/Boss", true, 1.0f, "GateBoss");
	door->AddClip("Deactivate", L"ProjectTextures/Gate/Chapter1/Boss", false, 1.0f, "GateBoss");

	door->AddClip("Activate", L"ProjectTextures/Gate/Chapter1/NPC", true, 1.0f, "GateNPC");
	door->AddClip("Deactivate", L"ProjectTextures/Gate/Chapter1/NPC", false, 1.0f, "GateNPC");
}

void ExitDoors::PlayDoor(int index, bool isOpen)
{
	const DoorInfo& info = infos[index];
	AnimObject* door = doorObjects[index];

	string state = (isOpen && !info.destroyed) ? "Activate" : "Deactivate";
	door->Play(GetGateTag(info) + state, door->Pos());
}

string ExitDoors::GetGateTag(const DoorInfo& info)
{
	if (info.destroyed) return "GateDestroyed";

	switch (info.room)
	{
	case RoomType::Shop:	return "GateNPC";
	case RoomType::Elite:	return "GateElite";
	case RoomType::Boss:	return "GateBoss";
	}

	switch (info.reward)
	{
	case RewardType::Skul:	return "GateGrave";
	case RewardType::Item:	return "GateChest";
	}

	return "GateNormal";
}
