#include "Framework.h"
#include "MultiPanelProgressBar.h"

MultiPanelProgressBar::MultiPanelProgressBar(const Textures& textures, Vector2 barSize)
{
	gauge = new Quad(textures.base);
	gauge->SetParent(this);
	gauge->SetPixelShader(L"Hpbar.hlsl");

	healthImage = Texture::Add(textures.health);
	healImage = LoadOptional(textures.heal);
	shieldImage = LoadOptional(textures.shield);
	damageImage = LoadOptional(textures.damage);

	amountBuffer = new FloatValueBuffer();

	SetBarSize(barSize);
	UpdateAmountBuffer();
}

MultiPanelProgressBar::~MultiPanelProgressBar()
{
	delete gauge;
	delete amountBuffer;
}

void MultiPanelProgressBar::Update()
{
	UpdateDamageTrail();
	UpdateHealFill();
	UpdateAmountBuffer();

	UpdateWorld();
	gauge->UpdateWorld();
}

void MultiPanelProgressBar::Render()
{
	//t0은 gauge(Quad)가 Render에서 자기 텍스처(base)로 채움
	healthImage->PSSet(1);
	BindLayer(healImage, 2);
	BindLayer(shieldImage, 3);
	BindLayer(damageImage, 4);
	amountBuffer->SetPS(1);

	gauge->Render();
}

void MultiPanelProgressBar::SetBarSize(Vector2 size)
{
	barSize = size;
	gauge->Scale() = size / gauge->GetSize();
}

void MultiPanelProgressBar::Init(float maxHP, float curHP)
{
	this->maxHP = max(maxHP, 1.0f);
	this->curHP = Clamp(0.0f, this->maxHP, curHP);

	shownHP = this->curHP;
	trailHP = this->curHP;
	damageDelayTimer = 0.0f;
	healDelayTimer = 0.0f;

	UpdateAmountBuffer();
}

void MultiPanelProgressBar::SetHP(float hp)
{
	hp = Clamp(0.0f, maxHP, hp);
	if (hp == curHP) return;

	if (changeMode == ChangeMode::INSTANT_CHANGE)
	{
		shownHP = curHP;
		trailHP = curHP;
	}

	bool damageMode = trailHP > curHP;
	bool healMode = curHP > shownHP;

	//데미지를 받았을때
	if (hp < curHP)
	{
		//trailHP는 curHP를 향해 간다. 체력색은 즉시 hp값으로 나머지는 trailHP가
		trailHP = max(trailHP, curHP);
		shownHP = min(shownHP, hp);

		if (!damageMode) damageDelayTimer = followDelay;
	}
	else
	{
		//힐을 받을때
		trailHP = max(trailHP, hp);

		if (!healMode) healDelayTimer = followDelay;
	}

	curHP = hp;
	UpdateAmountBuffer();
}

void MultiPanelProgressBar::SetMaxHP(float maxHP)
{
	this->maxHP = max(maxHP, 1.0f);
	curHP = min(curHP, this->maxHP);
	shownHP = min(shownHP, curHP);
	trailHP = Clamp(curHP, this->maxHP, trailHP);

	UpdateAmountBuffer();
}

void MultiPanelProgressBar::SetShield(float shield)
{
	this->shield = max(shield, 0.0f);
	UpdateAmountBuffer();
}

void MultiPanelProgressBar::UpdateDamageTrail()
{
	if (trailHP <= curHP) return;

	if (damageDelayTimer > 0.0f)
	{
		damageDelayTimer -= DELTA;
		return;
	}

	trailHP = max(curHP, trailHP - maxHP * followSpeed * DELTA);
}

void MultiPanelProgressBar::UpdateHealFill()
{
	if (shownHP >= curHP) return;

	if (healDelayTimer > 0.0f)
	{
		healDelayTimer -= DELTA;
		return;
	}

	shownHP = min(curHP, shownHP + maxHP * followSpeed * DELTA);
}

void MultiPanelProgressBar::UpdateAmountBuffer()
{
	//체력+실드가 최대 체력을 넘으면 바 전체를 그 합 기준으로 나눠 가짐
	float total = max(maxHP, curHP + shield);

	float healthEnd = shownHP / total;
	float healEnd = curHP / total;
	float shieldEnd = (curHP + shield) / total;
	float damageEnd = trailHP / total;

	//텍스처가 없는 층은 폭을 0으로 줄임 (힐 구간은 체력색으로 대신 채움)
	if (healImage == nullptr) healthEnd = healEnd;
	if (shieldImage == nullptr) shieldEnd = healEnd;
	if (damageImage == nullptr) damageEnd = 0.0f;

	float* values = amountBuffer->Get();
	values[0] = healthEnd;
	values[1] = healEnd;
	values[2] = shieldEnd;
	values[3] = damageEnd;
}

Texture* MultiPanelProgressBar::LoadOptional(const wstring& file)
{
	if (file.empty()) return nullptr;
	return Texture::Add(file);
}

void MultiPanelProgressBar::BindLayer(Texture* layer, UINT slot)
{
	//없는 층도 슬롯은 비워두지 않음 (다른 오브젝트가 꽂아둔 텍스처가 남아 있지 않게) - 폭이 0이라 보이지 않음
	if (layer == nullptr)
		layer = gauge->GetTexture();

	layer->PSSet(slot);
}
