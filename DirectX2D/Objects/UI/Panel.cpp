#include "Framework.h"

Panel::Panel(wstring bgTextureFile, wstring cancelTextureFile)
	:Quad(bgTextureFile)
{
	cancelTextureFile.length() > 1 ? hasCancel = true : hasCancel = false;

	if(hasCancel)
	{
		cancelButton = new Button(cancelTextureFile);
		cancelButton->SetParent(this);
		cancelButton->Pos() = size * 0.5f - cancelButton->GetSize() * 0.5f; //패널 오른쪽 위에 위치
		cancelButton->SetEvent(bind(&Panel::OnClickCancel, this));
	}

	collider = new RectCollider(size);
	collider->SetParent(this);

	isActive = false; // 시작 직후에 바로 UI가 뜨는 경우를 배제
}

Panel::~Panel()
{
	delete collider;
	if(cancelButton != nullptr)
		delete cancelButton;
}

void Panel::Update()
{
	if (!isActive) return;

	Drag();
	UpdateWorld();

	if(hasCancel)
		cancelButton->Update();
	collider->UpdateWorld();
}

void Panel::Render()
{
	if (!isActive) return;

	Quad::Render();

	if(hasCancel)
		cancelButton->Render();
	collider->Render();
}

void Panel::Show(Vector2 pos)
{
	Pos() = pos;
	isActive = true;
}

void Panel::Drag()
{
	if(hasCancel && cancelButton->GetCollider()->IsPointCollision(mousePos))
		return; // 취소 버튼 위에 마우스가 올라와 있으면 드래그 금지

	if (!collider->IsPointCollision(mousePos)) return;

	if (!isDrag) return;

	if(KEY_DOWN(VK_LBUTTON))
	{
		offset = mousePos - Pos();
	}

	if(KEY_PRESS(VK_LBUTTON))
	{
		Pos() = mousePos - offset;
	}

	if(KEY_UP(VK_LBUTTON))
	{
		isDrag = false;
	}
}

void Panel::OnClickCancel()
{
	isActive = false;
	PanelManager::Get()->Hide(tag);
}
