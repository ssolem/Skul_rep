#include "Framework.h"
#include "ButtonPuzzleScene.h"

// 오늘의 실습 : 우리가 처음 생성했던 그림 외에 다른 그림도 보고 싶다면 어떻게 하면 될까요?
//              #1. 다른 파일 이름도 줄 수 있을까요?
//              #2. 여러 짝을 만든 후, 일부(혹은 n번째 이후)만을 사용할 수도 있을까요?

// 실습 B : 카드 배치 시에, 혹시 '패딩'(공백 간격)도 설정 가능할까요?

ButtonPuzzleScene::ButtonPuzzleScene()
{
	//테스트 플레이용 랜덤 시드 생성
	srand(GetTickCount64());

	LoadTextures();
	ShuffleTextures();
	SetPadding(10, 10);
	CreatePairs(15);
	ShufflePairs();

	//-> 랜덤 버튼 퍼즐 판(사천성 판)이 만들어질 것
	//	* 층 같은 어려운 컨텐츠는 일단 생략

	Observer::Get()->AddParamEvent("ButtonAction",
		bind(&ButtonPuzzleScene::SelectButton, this, placeholders::_1));
}

ButtonPuzzleScene::~ButtonPuzzleScene()
{
	for (ButtonObject* button : buttons)
		delete button;
}

void ButtonPuzzleScene::Update()
{
	//if(firstSelect) //한 버튼을 골랐는데 시간을 너무 끈 경우 여기 포함
	if (firstSelect && secondSelect)
	{
		//짝을 맞췄든 아니든 그 결과가 다음에 영향을 주지 않게 카드 자료에서 삭제
		deleteTime += DELTA;

		if (deleteTime >= DELETE_DELAY_LIMIT)
			DeleteMatch();
	}

	for (ButtonObject* button : buttons)
		button->Update();
}

void ButtonPuzzleScene::Render()
{
	for (ButtonObject* button : buttons)
		button->Render();
}

void ButtonPuzzleScene::PostRender()
{
	if (ImGui::InputFloat4("OutlineColor", outlineColor))
	{
		for (ButtonObject* button : buttons)
		{
			button->SetNewOutlineColor(
				outlineColor[0], 
				outlineColor[1], 
				outlineColor[2], 
				outlineColor[3]);
		}
	}

	if (ImGui::SliderInt("OutlineScale", &outlineScale, 1, 15))
	{
		for (ButtonObject* button : buttons)
		{
			button->SetNewOutlineScale(outlineScale);
		}
	}
}

void ButtonPuzzleScene::Start()
{
}

void ButtonPuzzleScene::End()
{
}

void ButtonPuzzleScene::Reset()
{
}

void ButtonPuzzleScene::CreatePairs(UINT pairLevel)
{
	//전체 버튼이 몇 개 있을 것인가
	UINT totalButtonSize = pairLevel * PAIR_NUM;

	vector<UINT> divisors;	//인수
	//인수들 저장
	for (UINT i = 1; i <= totalButtonSize; ++i)
	{
		if (totalButtonSize % i == 0)
		{
			divisors.push_back(i);
		}
	}

	//이 인수의 중간을 찾으면 가장 보기에 자연스러운 (가장 풀기에 까다로운) 가로 세로 짝을 찾을 수 있게 됨.

	UINT halfDivisorSize = divisors.size() / 2;

	if (divisors.size() % 2 == 0) // 인수 홀짝은 전체 벡터의 홀짝과는 관계
	{
		width = divisors[halfDivisorSize];
		height = divisors[halfDivisorSize - 1];
	}
	else
	{
		width = divisors[halfDivisorSize];
		height = divisors[halfDivisorSize];
	}

	//그림 크기 + 전체 크기
	Vector2 size = Texture::Add(buttonKeys[0])->GetSize() * BUTTON_SCALE;
	Vector2 totalHalfSIze = size * Vector2(width, height) * 0.5f;
	Vector2 paddingSize = Vector2((width - 1) * paddingWidth, (height - 1) * paddingHeight) * 0.5f;
	//배치의 중앙부
	Vector2 startOffset = Vector2(CENTER_X, CENTER_Y) - totalHalfSIze + (size * 0.5f) - paddingSize;

	//버튼 생성 및 배치
	buttonBoard.resize(height); //높이 = 가로줄 개수
	for (vector<ButtonObject*>& buttons : buttonBoard)
		buttons.resize(width); //너비 ==

	for (UINT y = 0; y < height; ++y)
	{
		for (UINT x = 0; x < width; ++x)
		{
			Vector2 pos = startOffset + Vector2(x, y) * size + Vector2(x * paddingWidth, y * paddingHeight);

			UINT index = (y * width + x) / PAIR_NUM; // 현재 불러올 인덱스 (1차원화 / 짝 개수)
			ButtonObject* button = new ButtonObject(buttonKeys[index]); // 그 인덱스로 키 호출

			button->Pos() = pos;
			button->Scale() *= BUTTON_SCALE;
			button->SetCoord(x, y);

			buttons.push_back(button);
			buttonBoard[y][x] = button;
		}
	}
}

void ButtonPuzzleScene::LoadTextures()
{
	WIN32_FIND_DATA findData;
	HANDLE handle = FindFirstFile(L"Textures/Cards/*.png", &findData);

	bool found = true;
	wstring fileName;

	while (found)
	{
		fileName = L"Textures/Cards/";
		fileName += findData.cFileName;

		buttonKeys.push_back(fileName);

		found = FindNextFile(handle, &findData);
	}
}

void ButtonPuzzleScene::ShufflePairs()
{
	for (int i = 0; i < SHUFFLE; ++i)
	{
		int r1 = Random(0, buttons.size());
		int r2 = Random(0, buttons.size());

		SwapButton(r1, r2);
	}
}

void ButtonPuzzleScene::SwapButton(int n1, int n2)
{
	//배열이 바뀔 때 좌표도 같이 바뀌게
	Vector2 tempPos = buttons[n1]->Pos();
	buttons[n1]->Pos() = buttons[n2]->Pos();
	buttons[n2]->Pos() = tempPos;

	POINT tempCoord = buttons[n1]->GetCoord();
	buttons[n1]->SetCoord(buttons[n2]->GetCoord());
	buttons[n2]->SetCoord(tempCoord);

	POINT coord1 = buttons[n1]->GetCoord();
	POINT coord2 = buttons[n2]->GetCoord();

	// 배열상의 카드 정보까지 최종 치환
	ButtonObject* tempButton = buttonBoard[coord1.y][coord1.x];
	buttonBoard[coord1.y][coord1.x] = buttonBoard[coord2.y][coord2.x];
	buttonBoard[coord2.y][coord2.x] = tempButton;
}

void ButtonPuzzleScene::ShuffleTextures()
{
	for (int i = 0; i < SHUFFLE; ++i)
	{
		int r1 = Random(0, buttonKeys.size());
		int r2 = Random(0, buttonKeys.size());

		SwapTexture(r1, r2);
	}
}

void ButtonPuzzleScene::SwapTexture(int n1, int n2)
{
	wstring temp = buttonKeys[n1];
	buttonKeys[n1] = buttonKeys[n2];
	buttonKeys[n2] = temp;
}

//짝 맞춤 : 두 카드를 클릭 했을 때 #1 비교 #2(혹은#0) 클릭에 대한 반응 구현
bool ButtonPuzzleScene::IsSamePair() // 카드가 짝이 맞는가
{
	if (firstSelect == secondSelect) //완전히 같은 두 카드
	{
		MessageBox(hWnd, L"같은 카드입니다. 짝이 될 수 없습니다.", L"Error", MB_OK);
		return false;
	}

	if (firstSelect->GetKey() == secondSelect->GetKey()) //두 카드가 다른 카드인데 키가 같다
		return true;

	MessageBox(hWnd, L"서로 다른 카드입니다.", L"Error", MB_OK);
	return false;
}

bool ButtonPuzzleScene::IsSameLine(POINT coord1, POINT coord2)
{
	if (coord1.x != coord2.x && coord1.y != coord2.y)
		return false;

	// 행 과 열 중 하나는 일치함
	if (coord1.x == coord2.x)
	{
		int minY = min(coord1.y, coord2.y);
		int maxY = max(coord1.y, coord2.y);

		//누가 누구의 위인지는 묻지 않고 서로의 세로 범위만 파악한다.
		//해당 세로 범위를 진행하는 중에 장애물이 있는지만 확인

		for (int y = minY + 1; y < maxY; ++y)
		{
			if (buttonBoard[y][coord1.x]->Active())
				return false; //장애물에 막힘
		}
	}

	if (coord1.y == coord2.y)
	{
		int minX = min(coord1.x, coord2.x);
		int maxX = max(coord1.x, coord2.x);

		for (int x = minX + 1; x < maxX; ++x)
		{
			if (buttonBoard[coord1.y][x]->Active())
				return false; //장애물에 막힘
		}
	}

	return true;	//같은 행 또는 열인데 중간에 장애물이 없음 -> 같은 줄에 있음
}

bool ButtonPuzzleScene::IsSameLineIncludeSelf(POINT coord1, POINT coord2)
{
	//본인을 포함해서도 같은 줄인지 확인

	if (coord1.x != coord2.x && coord1.y != coord2.y) //가로세로 다름
		return false;

	if (coord1.x == coord2.x)
	{
		int minY = min(coord1.y, coord2.y);
		int maxY = max(coord1.y, coord2.y);

		for (int y = minY + 1; y <= maxY; ++y) //봐야 하는 대상에 목적지가 포함
		{
			if (buttonBoard[y][coord1.x]->Active())
				return false;
		}
	}

	if (coord1.y == coord2.y)
	{
		int minX = min(coord1.x, coord2.x);
		int maxX = max(coord1.x, coord2.x);

		for (int x = minX + 1; x <= maxX; ++x)
		{
			if (buttonBoard[coord1.y][x]->Active())
				return false;
		}
	}

	return true;
}

bool ButtonPuzzleScene::IsULine(POINT coord1, POINT coord2)
{
	//각자가 향한 '여유 방향'이 어디인지 파악

	EmptyDir result1 = GetEmptyDir(coord1);
	EmptyDir result2 = GetEmptyDir(coord2);

	if (result1.up && result2.up) return true;
	if (result1.down && result2.down) return true;
	if (result1.left && result2.left) return true;
	if (result1.right && result2.right) return true;

	//빈 방향 앞에 다른 장애물 여부 검사
	//오른쪽
	for (int x = coord1.x + 1; x < width; ++x)
	{
		POINT coord = { x, coord1.y };
		if (buttonBoard[coord.y][coord.x]->Active())
			break;

		if (IsSameLine(coord, coord2)) return true;

		POINT tempCoord = { x, coord2.y };

		if (IsSameLineIncludeSelf(coord, tempCoord)) //일단 목적지까지는 문제 없으면
		{
			//목적지에서 여기로 꺽어 오는 건 되는지 확인
			if (IsSameLine(coord2, tempCoord))
				return true;
		}
	}

	//왼쪽
	for (int x = coord1.x - 1; x >= 0; --x)
	{
		POINT coord = { x, coord1.y };
		if (buttonBoard[coord.y][coord.x]->Active())
			break;

		if (IsSameLine(coord, coord2)) return true;

		POINT tempCoord = { x, coord2.y };

		if (IsSameLineIncludeSelf(coord, tempCoord)) //일단 목적지까지는 문제 없으면
		{
			//목적지에서 여기로 꺽어 오는 건 되는지 확인
			if (IsSameLine(coord2, tempCoord))
				return true;
		}
	}
	//위
	for (int y = coord1.y + 1; y < height; ++y)
	{
		POINT coord = { coord1.x, y };
		if (buttonBoard[coord.y][coord.x]->Active())
			break;

		if (IsSameLine(coord, coord2)) return true;

		POINT tempCoord = { coord2.x, y };

		if (IsSameLineIncludeSelf(coord, tempCoord)) //일단 목적지까지는 문제 없으면
		{
			//목적지에서 여기로 꺽어 오는 건 되는지 확인
			if (IsSameLine(coord2, tempCoord))
				return true;
		}
	}

	//아래
	for (int y = coord1.y - 1; y >= 0; --y)
	{
		POINT coord = { coord1.x, y };
		if (buttonBoard[coord.y][coord.x]->Active())
			break;

		if (IsSameLine(coord, coord2)) return true;

		POINT tempCoord = { coord2.x, y };

		if (IsSameLineIncludeSelf(coord, tempCoord)) //일단 목적지까지는 문제 없으면
		{
			//목적지에서 여기로 꺽어 오는 건 되는지 확인
			if (IsSameLine(coord2, tempCoord))
				return true;
		}
	}

	// 여기 까지 왔다면 같은 방향으로 튼 것도 아닌데, 중간에 꺽여 연결될 가능성 조차 없음

	return false;
}

ButtonPuzzleScene::EmptyDir ButtonPuzzleScene::GetEmptyDir(POINT coord)
{
	EmptyDir result;

	//주어진 좌표에서 가로세로 쭉 가보면서 장애물이 있으면 false

	//오른쪽
	for (int x = coord.x + 1; x < width; ++x)
	{
		if (buttonBoard[coord.y][x]->Active())
			result.right = false;
	}

	//왼쪽
	for (int x = coord.x - 1; x >= 0; --x)
	{
		if (buttonBoard[coord.y][x]->Active())
			result.left = false;
	}

	//위
	for (int y = coord.y + 1; y < height; ++y)
	{
		if (buttonBoard[y][coord.x]->Active())
			result.up = false;
	}

	//아래
	for (int y = coord.y - 1; y >= 0; --y)
	{
		if (buttonBoard[y][coord.x]->Active())
			result.down = false;
	}

	return result;
}

void ButtonPuzzleScene::CheckMatch() //비교를 하고, 짝이 맞으면 어쩔 거냐
{
	if (!IsSamePair()) return; //짝이 아니면 비교도 안 한다.

	//두 카드를 골랐다고 치고 좌표간 비교 진행
	if (IsSameLine(firstSelect->GetCoord(), secondSelect->GetCoord())) //같은 선상
	{
		firstSelect->SetActive(false);
		secondSelect->SetActive(false);
		return;
	}

	//같은 방식으로 U자 적용
	if (IsULine(firstSelect->GetCoord(), secondSelect->GetCoord()))
	{
		firstSelect->SetActive(false);
		secondSelect->SetActive(false);
		return;
	}
}

void ButtonPuzzleScene::DeleteMatch() // 선택한 두 카드의 포인터 삭제 (리셋)
{
	isDeleteDelay = false;

	firstSelect->SetSelected(false);
	secondSelect->SetSelected(false);

	firstSelect = nullptr;
	secondSelect = nullptr;
}

void ButtonPuzzleScene::SelectButton(void* selectedButton) // 버튼을 '클릭한' 건 어떻게 할거냐
{
	if (isDeleteDelay)
		DeleteMatch();

	ButtonObject* target = (ButtonObject*)selectedButton;
	target->SetSelected(true); // 이 버튼을 찍었음을 기록

	//여기서 작성 시, 하나만 찍어도 삭제 카운트 시작
	//deleteTime = 0;
	//isDeleteDelay = true;

	if (firstSelect == nullptr) //아직 두 버튼 중 아무것도 안 고른 상태
	{
		firstSelect = target;
		return;
	}

	//첫번째 버튼을 선택한 상태

	secondSelect = target;

	// 두 버튼이 준비됐으니 짝 맞춤 진행
	CheckMatch();

	//여기서 작성 시, 둘다 찍어야 효력
	deleteTime = 0;
	isDeleteDelay = true;
}
