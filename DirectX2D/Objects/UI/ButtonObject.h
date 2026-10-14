#pragma once

class ButtonObject : public Button
{
public:
	ButtonObject(wstring textureFile);
	~ButtonObject();

	void Update();
	void Render();

	void SetNewOutlineColor(float r, float g, float b, float a);
	void SetNewOutlineScale(int scale) { outlineScale = scale; }

	void SetCoord(int x, int y) { coord = { x, y }; }
	void SetCoord(POINT coord) { this->coord = coord; }
	POINT GetCoord() { return coord; }

	void SetSelected(bool value) { isSelected = value; }

	void SetKey(wstring key) { this->key = key; }
	wstring GetKey() { return key; }

	// 데이터에 겟셋터가 있으면 : 데이터 안전은 보장이 안 돼도
	//                          사후 대응 능력은 보장 가능
	
private:
	void OnClick();			// 이벤트 등록, 혹은 호출용 함수

private:
	// 씬 등에 같은 종류가 여럿 있을 때 (메뉴창, 퍼즐 게임)
	// 각 오브젝트를 지칭하기 위한 부가 데이터들

	wstring key;	// 같은 객체가 여럿 나올 때 지칭하는 호출명
	POINT coord;	// 나열 식으로 같은 객체가 여럿 나올 때
					// 상대 위치를 지정하는 변수

	bool isSelected = false;	// 버튼이 토글식일 때
								// (온오프 옵션, 짝맞춤 게임 등)

	// -----------------------------------------------------------
	
	//외곽선(아웃라인) 적용을 위한 상수 버퍼
	IntValueBuffer* intValueBuffer;
	FloatValueBuffer* floatValueBuffer;
	ColorBuffer* outlineColorBuffer;

	int outlineScale = 1;	// 외곽선 두께
};

