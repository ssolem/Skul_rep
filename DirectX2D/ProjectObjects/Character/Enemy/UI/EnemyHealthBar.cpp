#include "Framework.h"
#include "EnemyHealthBar.h"
#include "Objects/UI/NineSliceImage.h"
#include "Objects/UI/MultiPanelProgressBar.h"

EnemyHealthBar::EnemyHealthBar(Vector2 barSize)
{
	wstring path = L"ProjectTextures/UI/HealthBar_Enemy/";

	MultiPanelProgressBar::Textures textures;
	textures.base = path + L"EnemyHealthBar_Base.png";
	textures.health = path + L"EnemyHealthBar.png";
	textures.heal = path + L"EnemyHealthBar_Heal.png";
	textures.shield = path + L"EnemyHealthBar_Shield.png";
	textures.damage = path + L"EnemyHealthBar_Damage.png";

	gauge = new MultiPanelProgressBar(textures, barSize);
	gauge->SetParent(this);

	frame = new NineSliceImage(path + L"EnemyHealthBar_Frame.png", Vector2(3.0f / 7.0f, 2.0f / 5.0f));
	frame->SetParent(this);

	SetBarSize(barSize);
}

EnemyHealthBar::~EnemyHealthBar()
{
	delete gauge;
	delete frame;
}

void EnemyHealthBar::Update()
{
	UpdateWorld();
	gauge->Update();
	frame->Update();
}

void EnemyHealthBar::Render()
{
	gauge->Render();
	frame->Render();	// 게이지 위에 덮음 - 가운데 투명한 창으로 게이지 색이 보임
}

Vector2 EnemyHealthBar::GetBarSize()
{
	return gauge->GetBarSize();
}

void EnemyHealthBar::SetBarSize(Vector2 size)
{
	gauge->SetBarSize(size);

	//프레임 전체 높이 = 게이지 높이 (위아래 모퉁이 높이를 뺀 나머지만 늘림)
	frame->SetCenterSize(Vector2(size.x, size.y - frame->GetCornerSize().y * 2.0f));
}

void EnemyHealthBar::Init(float maxHP, float curHP)
{
	gauge->Init(maxHP, curHP);
}

void EnemyHealthBar::SetHP(float hp)
{
	gauge->SetHP(hp);
}

void EnemyHealthBar::SetMaxHP(float maxHP)
{
	gauge->SetMaxHP(maxHP);
}

void EnemyHealthBar::SetShield(float shield)
{
	gauge->SetShield(shield);
}
