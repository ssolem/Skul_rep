#pragma once
class NineSliceImage;
class MultiPanelProgressBar;

//적 체력바 = 게이지(MultiPanelProgressBar) + 9분할 프레임
//체력/연출 로직은 게이지가 담당하고, 여기서는 적 전용 텍스처와 프레임 배치만 맡음

class EnemyHealthBar : public GameObject
{
public:
	EnemyHealthBar(Vector2 barSize);
	~EnemyHealthBar();

	void Update();
	void Render();

	Vector2 GetBarSize();
	void SetBarSize(Vector2 size);

	void Init(float maxHP, float curHP);    // 연출 없이 즉시 맞춤
	void SetHP(float hp);                   // 줄면 데미지 연출, 늘면 힐 연출
	void SetMaxHP(float maxHP);
	void SetShield(float shield);           // 연출 없음

	MultiPanelProgressBar* GetGauge() { return gauge; }	// 연출 모드/속도 등 세부 설정용

private:
	MultiPanelProgressBar* gauge;	// 게이지 (base/health/heal/shield/damage)
	NineSliceImage* frame;			// 게이지 위에 덮는 테두리
};
