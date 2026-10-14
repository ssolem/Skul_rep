#pragma once

class PlayerInfoSlot : public Transform
{
public:
	PlayerInfoSlot(wstring frameFile, Vector2 iconOffset, Vector2 iconSize);
	~PlayerInfoSlot();

	void Update();
	void Render(bool forceShow = false);	//forceShow: 아이콘이 없어도 프레임을 그림 (배치 조정용)
	void RenderUI();						//슬롯 위치 + 슬롯 안 아이콘 위치

	void SetIcon(Texture* texture);
	void SetDim(bool dimmed);
	//void SetCoolDown(float cooldown) { this->cooldown = cooldown; }

private:
	Quad* frame;
	Quad* icon;

	//float cooldown;
};

