#include "Framework.h"

PanelManager::PanelManager()
{
}

PanelManager::~PanelManager()
{
	for (pair<string, Panel*> panel : panels)
		delete panel.second;
}

void PanelManager::Update()
{
	CloseLastUI();
	SelectUI();

	for (pair<string, Panel*> panel : panels)
		panel.second->Update();
}

void PanelManager::Render()
{
	for (Panel* panel : popupList)
		panel->Render();

	for (pair<string, Panel*> panel : panels)
		panel.second->RenderUI();
}

void PanelManager::AddPanel(string key, wstring panelTextureFile, wstring cancelTextureFile)
{
	if (panels.count(key) > 0) return;

	Panel* panel = new Panel(panelTextureFile, cancelTextureFile);
	panel->SetTag(key); //키가 곧 트랜스폼의 태그가 되도록 (호출에 사용될 수 있게)
	panel->Load(); //트랜스폼 데이터가 있으면 불러오기
	panels[key] = panel;
}

void PanelManager::AddPanel(string key, wstring panelTextureFile, bool hasCancel)
{
	switch (hasCancel)
	{
	case true: AddPanel(key, panelTextureFile);
		break;
	case false: AddPanel(key, panelTextureFile, L"");
		break;
	}
}

void PanelManager::Show(string key)
{
	if (panels.count(key) == 0) return;

	panels[key]->SetActive(true);
	popupList.push_back(panels[key]);
	frontPanel = panels[key]; //가장 마지막에 열림 -> 가장 앞에 있는 패널
}

void PanelManager::Hide(string key)
{
	if (panels.count(key) == 0) return;

	//반복자 참조 기능으로 해당되는 원소를 리스트에서 찾는다.
	list<Panel*>::iterator it = find(popupList.begin(), popupList.end(), panels[key]);
	//std::advance() : 실제 임의접근부터 비트연산까지, 반복자에 대한 연산을 수행하는 함수		//std::advance(it, 1); //it를 1만큼 이동시	
	// 더불어 find에서 원소가 없을 경우 결과가 end()로 반환된다.

	if (it == popupList.end()) return; //리스트에 없으면 종료

	popupList.erase(it); //리스트에서 제거
	//popupList.remove(panels[key]);

	if (frontPanel == panels[key]) // 어떤 이유든 제일 위에 있던 패널이 닫히려고 하면
	{
		if (!popupList.empty())
			frontPanel = popupList.back(); //가장 마지막에 열림 -> 가장 앞에 있는 패널
		else
			frontPanel = nullptr;
	}
}

void PanelManager::SelectUI()
{
	if (KEY_DOWN(VK_LBUTTON))
	{
		if (popupList.empty()) return;

		list<Panel*>::reverse_iterator iter = popupList.rbegin(); //가장 마지막에 열린 패널부터 확인

		for (iter; iter != popupList.rend(); ++iter)
		{
			if ((*iter)->GetCollider()->IsPointCollision(mousePos))
			{
				if (*iter != frontPanel)
				{
					frontPanel = *iter;
					popupList.erase(--iter.base());
					popupList.push_back(frontPanel);
				}

				//클릭이 끝났으니 반복문 종료
				break;
			}
		}
	frontPanel->SetiIsDrag(true); //마우스 클릭시 드래그 가능하도록
	}

}

void PanelManager::CloseLastUI() //마지막 창 비우기
{
	if (KEY_DOWN(VK_ESCAPE))
	{
		if (!popupList.empty())
		{
			popupList.pop_back();

			if(popupList.empty())
				frontPanel = nullptr;
			else
				frontPanel = popupList.back();
		}
	}
}
