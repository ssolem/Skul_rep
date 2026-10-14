#include "Framework.h"
#include "TileMapEditScene.h"

//길찾기 진행이 되는 동안, 각자 마음에 드는 게임의 탑뷰 배경 레벨(스테이지)을 만들어봅시다.

//-> 해당 맵을 "TextData"폴더의 "Test.map" 파일로 저장.
//	+ 맵을 여러개 만들자.

TileMapEditScene::TileMapEditScene()
{
	editTileMap = new EditTileMap(10, 10);
	editTileMap->Pos() = { 80, 80 };

	CreateSample();

	selectedSample = new Quad(Vector2(SAMPLE_SIZE, SAMPLE_SIZE));

	char path[128];
	GetCurrentDirectoryA(sizeof(path), path);
	projectPath = path;
}

TileMapEditScene::~TileMapEditScene()
{
	delete editTileMap;
	delete selectedSample;
	for (Button* tile : sampleTiles) delete tile;
}

void TileMapEditScene::Update()
{
	//조작 (임시)

	if (KEY_PRESS(VK_LBUTTON) && !ImGui::GetIO().WantCaptureMouse) //디버그 창에 마우스 없을 때
	{
		if (selectedSample->GetTexture()) //미리 선택해둔 샘플이 있을 때만
			editTileMap->ClickTile(selectedSample, selectType);
	}

	if (KEY_DOWN(VK_RBUTTON) && !ImGui::GetIO().WantCaptureMouse)
	{
		editTileMap->DeleteTile((Tile::Type)selectType);
	}

	editTileMap->Update();

	for (Button* tile : sampleTiles) tile->Update();

	if (selectedSample->GetTexture()) // 일단 맵 연산은 끝내놓고 샘플이 있다면
	{
		if (mouseWheel > 0)
		{
			selectedSample->Rot().z += XM_PIDIV2; //90도 반시계 회전
			if (selectedSample->Rot().z > XM_2PI)
				selectedSample->Rot().z = XM_2PI;
		}
		else if (mouseWheel < 0)
		{
			selectedSample->Rot().z -= XM_PIDIV2; //90도 반시계 회전
			if (selectedSample->Rot().z < 0)
				selectedSample->Rot().z = 0;
		}

		selectedSample->Pos() = mousePos;
		selectedSample->UpdateWorld();
	}
}

void TileMapEditScene::Render()
{
	editTileMap->Render();
	for (Button* tile : sampleTiles) tile->Render();

	if (selectedSample->GetTexture())
		selectedSample->Render();
}

void TileMapEditScene::PostRender()
{
	//디버그 창에서 편집기의 타일 분류를 지정할 수 있게
	const char* list[] = { "BG", "OBJ", "BUSH" };
	//디버그창의 선택 메뉴 기능 호출, 생성
	ImGui::ListBox(
		"Type", //생성한 기능을 지칭하는 키
		(int*)&selectType, //수정하려는 데이터
		list,				//적용하는 선택지
		2);

	Save();
	Load();
}

void TileMapEditScene::ClickSampleTile(void* sampleTile)
{
	// 메뉴에서 샘플을 클릭하면 커서에 저장해둔다.(선택의 효과)
	Button* tile = (Button*)sampleTile;		//액션과 마찬가지
	selectedSample->SetTexture(tile->GetTexture());
}

void TileMapEditScene::CreateSample()
{
	// winAPI 기능 소개 : WIN32(64)_FIND_DATA 구조체를 이용해서 폴더 안의 파일들을 검색할 수 있다.
	WIN32_FIND_DATA findData;		// <- OS의 기능 역시 필요하면 가져올 수 있다는 사례
	HANDLE handle = FindFirstFile(L"Textures/Tile/*.png", &findData); //파일 검색 후, 맨 위의 파일 정보 반환

	//위에서 찾은 '파일의 데이터'로부터 이름 등 정보를 가져오기

	bool result = true;
	wstring fileName;

	UINT width = 10; //샘플 항목의 가로 개수(임의의 값)
	Vector2 size(SAMPLE_SIZE, SAMPLE_SIZE);

	Vector2 startPos(WIN_WIDTH - size.x, WIN_HEIGHT - size.y); //우측정렬(우상정렬)

	while (result)
	{
		fileName = L"Textures/Tile/";
		fileName += findData.cFileName;

		result = FindNextFile(handle, &findData); //다음 파일 검색

		//새 샘플이 벡터에서 차지할 '다음' 인덱스
		UINT index = sampleTiles.size();

		//샘플이 위치할 좌표
		Vector2 pos(index % width, index / width);

		//실제 샘플 버튼 생성
		Button* tile = new Button(size);
		tile->SetTexture(fileName);
		tile->Pos() = startPos - (pos * size);

		//타일 이벤트 등록
		tile->SetParamEvent(bind(&TileMapEditScene::ClickSampleTile, this, placeholders::_1));
		tile->SetObject(tile);

		//다 만든 타일은 벡터에 추가
		sampleTiles.push_back(tile);
	}
}

void TileMapEditScene::Save()
{
	//세이브와 로드는 디버그 창의 탐색기 기능을 사용

	//ImGui::Button("Save"); //save글자가 적힌 버튼 생성 버튼을 누르면 true 반환

	if (ImGui::Button("Save"))
		DIALOG->OpenDialog(		//DiaLog : Dialogue + log - 간단히 말하면 팝업 탐색창
			"Save",				//생성하는 팝업창의 객체 내 구분자(key)
			"Save File",		//창의 이름
			".map",				//탐색하는 기본 확장자
			"."				//파일 이름, 확장자, 복수 항목 등의 구분자
		);

	//위 처럼 입력하면 'Save' 버튼을 눌러 맵 저장 탐색창을 띄울 수 있다.

	if (DIALOG->Display("Save")) //디버그 창에 'Save'라는 키의 창이 떠 있다면
	{
		//DIALOG->IsOk(); //Ok 버튼이 있을 때, 누를 수 있다는 의미

		if (DIALOG->IsOk())
		{
			string file = DIALOG->GetFilePathName();
			file = file.substr(projectPath.length() + 1); // 경로명 + '\' 이후부터 가져올 수 있음

			//파일명만 남은 문자열을 save함수에 넣어줌
			editTileMap->Save(file);
		}

		DIALOG->Close(); // 탐색창에 닫기 버튼이 추간된다.(+누르면 닫힘)
	}
}

void TileMapEditScene::Load()
{
	if (ImGui::Button("Load"))
		DIALOG->OpenDialog("Load", "Load File", ".map", ".");

	if (DIALOG->Display("Load")) 
	{
		if (DIALOG->IsOk())
		{
			string file = DIALOG->GetFilePathName();
			file = file.substr(projectPath.length() + 1);

			editTileMap->Load(file);
		}

		DIALOG->Close();
	}
}
