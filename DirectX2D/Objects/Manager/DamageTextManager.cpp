#include "Framework.h"
#include "DamageTextManager.h"

DamageTextManager::DamageTextManager()
{
}

DamageTextManager::~DamageTextManager()
{
}

void DamageTextManager::Update()
{
	for (DamageText& text : texts)
	{
		if (!text.isActive) continue;

		text.time += DELTA;
		text.worldPos.y += RISE_SPEED * DELTA;

		if (text.time >= LIFE_TIME)
			text.isActive = false;
	}
}

void DamageTextManager::Render()
{
	Font* font = Font::Get();

	for (DamageText& text : texts)
	{
		if (!text.isActive) continue;

		float rate = text.time / LIFE_TIME;
		//서서히 fadeout (LIFE_TIME 과 같아지면 0.0f)
		float opacity = rate < FADE_START ? 1.0f : 1.0f - (rate - FADE_START) / (1.0f - FADE_START);

		float scale = text.isCritical ? 1.5f : 1.0f;

		Vector2 screenPos = CAM->WorldToScreen(text.worldPos);

		string color = "";

		if(!text.isPlayer)
		{
			switch (text.type)
			{
			case DamageType::Physical:
				color = "Orange";
				break;
			case DamageType::Magic:
				color = "Blue";
				break;
			case DamageType::Fixed:
				color = "White";
				break;
			}
		}
		else
			color = "Red";

		font->RenderText(to_string(text.damage), {screenPos.x, screenPos.y}, Float2(),
			text.isCritical? "DamageCritical" : "Damage", color);
	}
}

void DamageTextManager::Play(Vector2 worldPos, int amount, DamageType type, bool critical, bool isPlayer)
{
	DamageText* slot = nullptr;

	for (DamageText& text : texts)
	{
		if (!text.isActive)
		{
			slot = &text;
			break;
		}
	}

	if (slot == nullptr)
	{
		texts.emplace_back();
		slot = &texts.back();
	}

	slot->damage = amount;
	slot->worldPos = worldPos + Vector2(Random(-15.0f, 15.0f), 0.0f);
	slot->type = type;
	slot->isCritical = critical;
	slot->isPlayer = isPlayer;
	slot->time = 0.0f;
	slot->isActive = true;
}

void DamageTextManager::Clear()
{
	texts.clear();
}
