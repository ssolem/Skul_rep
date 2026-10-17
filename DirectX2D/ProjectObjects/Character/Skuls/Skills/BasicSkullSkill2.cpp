#include "Framework.h"
#include "BasicSkullSkill2.h"
#include "ProjectObjects/Character/Character.h"
#include "../BasicSkul.h"

BasicSkullSkill2::BasicSkullSkill2(Character* owner, float coolDown, BasicSkul* basicSkul)
	: Skill(owner, coolDown), basicSkul(basicSkul)
{
	type = AttackActionType::None;
}

BasicSkullSkill2::~BasicSkullSkill2()
{
}

void BasicSkullSkill2::Init()
{
	head = basicSkul->GetHead();
}

void BasicSkullSkill2::Start()
{
	isReady = false;
	usedSkill = true;

	owner->Pos() = head->Pos();
	owner->SetVelocity(Vector2::Zero);

	basicSkul->RetrieveHead();

	Finish();
}

void BasicSkullSkill2::End()
{
}

bool BasicSkullSkill2::IsReady()
{
	return isReady && head->Active();
}

void BasicSkullSkill2::SetIcon()
{
	icon = Texture::Add(L"ProjectTextures/Skul_Images/Rebone.png");
}
