#include "Framework.h"

EffectManager::~EffectManager()
{
	for (pair<string, Effects> effects : totalEffects) // 각 종류
	{
		for (Effect* effect : effects.second) // 풀 안의 오브젝트 찾기
			delete effect;					  // 다 지우기
	}
}

void EffectManager::Update()
{
	for (pair<string, Effects> effects : totalEffects)
	{
		for (Effect* effect : effects.second)
			effect->Update();
	}
}

void EffectManager::Render()
{
	for (pair<string, Effects> effects : totalEffects)
	{
		for (Effect* effect : effects.second)
			effect->Render();
	}
}

void EffectManager::Add(string key, UINT poolSize, wstring textureFile,
	float maxFrameX, float maxFrameY, float speed,	bool isAdditive)
{
	if (totalEffects.count(key) > 0) return;

	Effects effects(poolSize);	// 이펙터생성 (풀 생성)

	for (Effect*& effect : effects)
	{
		effect = new Effect(textureFile,
			Vector2(maxFrameX, maxFrameY),
			speed, isAdditive);
	}

	totalEffects[key] = effects;
	descs[key] = { textureFile, Vector2(maxFrameX, maxFrameY), speed, isAdditive };
}

Effect* EffectManager::Expand(const string& key)
{
	const EffectDesc& desc = descs[key];
	Effects& effects = totalEffects[key];

	Effect* effect = new Effect(desc.textureFile, desc.maxFrame, desc.speed, desc.isAdditive);

	//SetScale/SetColor/SetFrameDelay로 바꾼 값은 기존 풀에만 적용돼 있으므로 첫 번째 것에서 복사
	if (!effects.empty())
		effect->CopySettings(effects[0]);

	effects.push_back(effect);
	return effect;
}


Effect* EffectManager::Play(string key, Vector2 pos, bool flipX)
{
	if (totalEffects.count(key) == 0) return nullptr;

	for (Effect* effect : totalEffects[key])
	{
		if (!effect->Active())
		{
			effect->Scale().x = flipX ? abs(effect->Scale().x) : -abs(effect->Scale().x);
			effect->Play(pos);
			return effect;
		}
	}

	//전부 재생 중이면 풀을 늘림
	Effect* effect = Expand(key);
	effect->Scale().x = flipX ? abs(effect->Scale().x) : -abs(effect->Scale().x);
	effect->Play(pos);
	return effect;
}

void EffectManager::SetColor(string key, float r, float g, float b, float a)
{
	if (totalEffects.count(key) == 0) return;

	for (Effect* effect : totalEffects[key])
		effect->GetColor() = { r, g, b, a };
}

void EffectManager::SetScale(string key, Vector2 scale)
{
	if (totalEffects.count(key) == 0) return;

	for (Effect* effect : totalEffects[key])
	{
		effect->Scale() = scale;
	}
}

void EffectManager::SetFrameDelay(string key, UINT frame, float delay)
{
	if (totalEffects.count(key) == 0) return;

	for (Effect* effect : totalEffects[key])
		effect->SetFrameDelay(frame, delay);
}
