#pragma once

class PanelManager : public Singleton<PanelManager>
{
private:
	friend class Singleton;

	PanelManager();
	~PanelManager();

public:
	void Update();
	void Render();

	void AddPanel(string key, wstring panelTextureFile, wstring cancelTextureFile = L"Textures/Tile/Characters/red_character.png"); // 패널 추가
	void AddPanel(string key, wstring panelTextureFile, bool hasCancel); //true면 기본취소버튼이 있고, false면 취소버튼이 없는 패널

	void Show(string key);
	void Hide(string key);

	Panel* GetFrontPanel() { return frontPanel; } // 가장 앞에 있는 패널을 반환

private:
	void SelectUI();
	void CloseLastUI();

private:
	map<string, Panel*> panels; // 패널 이름과 패널 객체를 연결

	//활성화된 패널의 리스트
	list<Panel*> popupList; // 활성화된 패널을 순서대로 관리

	Panel* frontPanel = nullptr; // 가장 앞에 있는 패널을 가리키는 포인터

};