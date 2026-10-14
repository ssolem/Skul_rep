#pragma once

class Panel : public Quad
{
public:
	Panel(wstring bgTextureFile, wstring cancelTextureFile = L"");
	~Panel();

	void Update();
	void Render();

	void Show(Vector2 pos); //지정 위치에 노출, 화면에 띄우기

	void SetiIsDrag(bool value) { isDrag = value; }

	RectCollider* GetCollider() { return collider; }

private:
	void Drag(); //클릭후 드래그하는 기능
	void OnClickCancel(); //취소 버튼 클릭시 패널 닫기


private:
	bool hasCancel;
	Button* cancelButton;

	RectCollider* collider; //패널이 꼭 버튼은 아닌데, 충돌 감지는 되면 좋으므로

	Vector2 offset;
	bool isDrag = false; //드래그나 클릭을 통한 일시적 조작에 쓸 수도 있기 떄문

};

