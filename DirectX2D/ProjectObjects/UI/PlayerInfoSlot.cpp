#include "Framework.h"
#include "PlayerInfoSlot.h"

PlayerInfoSlot::PlayerInfoSlot(wstring frameFile, Vector2 iconOffset, Vector2 iconSize)
{
	frame = new Quad(frameFile);
	icon = new Quad(iconSize);
	frame->SetParent(this);
	icon->SetParent(this);
	icon->Pos() += iconOffset;

	amountBuffer = new FloatValueBuffer();
	amountBuffer->Get()[0] = 1.0f;
	icon->SetPixelShader(L"CoolDown.hlsl");

	SetActive(false);
}

PlayerInfoSlot::~PlayerInfoSlot()
{
	delete frame;
	delete icon;
	delete amountBuffer;
}

void PlayerInfoSlot::Update()
{
	UpdateWorld();
	frame->UpdateWorld();
	icon->UpdateWorld();
}

void PlayerInfoSlot::Render(bool forceShow)
{
	if (!Active() && !forceShow) return;
	frame->Render();
	if (icon->GetTexture())
	{
		amountBuffer->SetPS(1);
		icon->Render();
	}
}

void PlayerInfoSlot::RenderUI()
{
	Transform::RenderUI();

	//아이콘 태그는 슬롯 태그를 따라감 - SetTag가 가상 함수가 아니라 여기서 맞춤
	icon->SetTag(tag + "_Icon");
	icon->RenderUI();
}

void PlayerInfoSlot::SetIcon(Texture* texture)
{
	SetActive(texture != nullptr);
	if (texture)
	{
		icon->SetTexture(texture);
	}
}

void PlayerInfoSlot::SetDim(bool dimmed)
{
	float value = dimmed ? 0.6f : 1.0f;
	icon->GetColor() = Float4(value, value, value, 1.0f);
}
