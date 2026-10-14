#include "Framework.h"
#include "ButtonScene.h"

// 도전과제 : 이 씬에서 퍼즐 게임도 만들 수 있을까요? 
//           여유가 된다면, 해봅시다.

// 예시 #1 : 짝맞추기 게임 (화면은 가려지면 더 좋습니다)
// 예시 #2 : 사천성 (까다로운 경우)

// 두 예시 모두 한 가지 공통점을 공유합니다.
// -> 복수의 같은 카드가 나온다는 것. (특히 그 중에서도 짝수로)

ButtonScene::ButtonScene()
{
	// 윈도우 기능을 써서 파일 이름 추출

	WIN32_FIND_DATA findData;
	HANDLE handle = FindFirstFile(
		L"Textures/Cards/*.png", &findData);
	// -> 폴더 안의 모든 파일을 검색한 후 결과를 검색결과에 저장
	// -> 그 폴더의 첫 번째 파일 이름이 들어간다

	bool found = true;
	wstring fileName;

	while (found)
	{
		fileName = L"Textures/Cards/"; // 경로명 미리 저장
		fileName += findData.cFileName;

		buttonKeys.push_back(fileName);

		found = FindNextFile(handle, &findData);
			// -> 검색 결과에서 그 다음 검색의 결과
	}
	// -------- 버튼의 생성과 이름 작성에 필요한 문자열이 나온다

	buttons.resize(size);

	for (int i = 0; i < size; i++)
	{
		buttons[i] = new ButtonObject(buttonKeys[i]);
		buttons[i]->Scale() *= 0.5f;

		buttons[i]->Pos() =
		{
			WIN_WIDTH * 0.2f * (i % width + 1)  ,
			WIN_HEIGHT * 0.2f * (i / width + 1),
		};
	}

	//--------------------------------------------
	//button = new Button(L"Textures/sun.png");
	//button->Pos() = { CENTER_X, CENTER_Y };
}

ButtonScene::~ButtonScene()
{
	// delete button;

	for (ButtonObject* button : buttons)
		delete button;
}

void ButtonScene::Update()
{
	// button->Update();

	for (ButtonObject* button : buttons)
		button->Update();
}

void ButtonScene::Render()
{
	// button->Render();

	for (ButtonObject* button : buttons)
		button->Render();
}

void ButtonScene::PostRender()
{
}
