#pragma once
class ButtonPuzzleScene : public Scene
{
private: // 게임 구성 및 기초 규칙
	struct EmptyDir
	{
		bool up = true;
		bool down = true;
		bool right = true;
		bool left = true;
	};

	const UINT PAIR_NUM = 4; //짝 구성 개수, 홀수 엄금, 2개도 웬만하면 비추천
	const float BUTTON_SCALE = 0.5f; //한 화면에 버튼이 여러개일 것이므로 크기 조정
	const UINT SHUFFLE = 1000;

	const float DELETE_DELAY_LIMIT = 0.5f;

public:
	ButtonPuzzleScene();
	~ButtonPuzzleScene();

	virtual void Update() override;
	virtual void Render() override;
	virtual void PostRender() override;

	virtual void Start() override;
	virtual void End() override;
	virtual void Reset() override;

private:
	void CreatePairs(UINT pairLevel);		// 짝의 '레벨' : 몇 ' 단위'가 나올 것인가
	void LoadTextures();					//리소스 로드 (그림 준비) 및 키 등록
	void ShufflePairs();
	void SwapButton(int n1, int n2);
	void ShuffleTextures();
	void SwapTexture(int n1, int n2);

	bool IsSamePair();
	bool IsSameLine(POINT coord1, POINT coord2);
	bool IsSameLineIncludeSelf(POINT coord1, POINT coord2);
	bool IsULine(POINT coord1, POINT coord2);

	EmptyDir GetEmptyDir(POINT coord);

	void CheckMatch();
	void DeleteMatch();

	void SelectButton(void* selectedButton); //버튼 자료형 대응 + 함수 포인터에 등록 예정
	void SetPadding(float width, float height) { paddingWidth = width; paddingHeight = height; }

private:

	vector<wstring> buttonKeys;	
	vector<ButtonObject*> buttons;
	vector<vector<ButtonObject*>> buttonBoard; //이중 벡터 (버튼 판)

	UINT width, height;
	
	ButtonObject* firstSelect = nullptr; //짝 맞춤에 쓸 두 버튼
	ButtonObject* secondSelect = nullptr;

	float deleteTime = 0;
	bool isDeleteDelay = false;

	float paddingWidth = 0.0f, paddingHeight = 0.0f;

	float outlineColor[4] = { 0, 0, 0, 1 };
	int outlineScale = 1;
};

