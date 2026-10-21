#pragma once

class PlayerInfoSlot;
class MultiPanelProgressBar;
class Player;
class Skill;
class SkulClass;

class PlayerInfoUI : public GameObject
{
public:
	PlayerInfoUI(Player* player);
	~PlayerInfoUI();

	void Update();
	void Render();
	void RenderUI();	//각 요소 위치를 ImGui로 조정 (PostRender에서 호출)

	//EnemyHealthBar와 같은 인터페이스 - 값은 Character::SetOnHPChanged 콜백으로 들어옴
	void Init(float maxHP, float curHP);    // 연출 없이 즉시 맞춤
	void SetHP(float hp);                   // 줄면 데미지 연출, 늘면 힐 연출
	void SetMaxHP(float maxHP);
	void SetShield(float shield);           // 연출 없음

private:
	void Refresh();
	Texture* GetSkillIcon(SkulClass* skul, int index);

	void CreateCounters();
	void UpdateCounters();
	void RenderCounters();

private:
	Player* player;
	Quad* mainFrame = nullptr;
	Quad* mainPortrait = nullptr;
	Quad* firstSkillIcon = nullptr;
	MultiPanelProgressBar* gauge = nullptr;

	PlayerInfoSlot* secondSkillIcon = nullptr;

	PlayerInfoSlot* subPortrait = nullptr;
	PlayerInfoSlot* firstSubSkillIcon = nullptr;
	PlayerInfoSlot* secondSubSkillIcon = nullptr;
	PlayerInfoSlot* essenceIcon = nullptr;

	bool isSecondSkillActive = false;
	bool isSubSkulActive = false;
	bool isSecondSubSkillActive = false;
	bool isEssenceActive = false;

	bool showAllSlots = false;	//디버그: 빈 슬롯도 프레임을 그려서 위치 확인

	enum CounterType { ENEMY, GOLD, DARK_QUARTZ, BONE_CHIP, COUNTER_MAX };

	Transform* counterRoot = nullptr;
	Quad* counterIcons[COUNTER_MAX] = {};
	int counterValues[COUNTER_MAX] = {};
	float counterRowGap = 12.0f;					
	Vector2 counterTextOffset = Vector2(16.0f, 12.0f);	
	Vector2 counterTextSize = Vector2(100.0f, 24.0f);
};

