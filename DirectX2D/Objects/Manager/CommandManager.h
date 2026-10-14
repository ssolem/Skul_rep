#pragma once

// 커맨드 관리자 : 복잡한 입력이 들어오면 무슨 의미인지 찾아주는 전역 클래스

class CommandManager : public Singleton<CommandManager>
{
private:
	friend class Singleton;

	CommandManager();
	~CommandManager() = default;

public:

	void Update();	// 상시관리
	void GetInput(int key);	// 따로 입력이 들어오는 경우

private:

	void PushCommand(int key); // 큐에 입력 넣기
	void CheckCommand(); // 기술이 나갈 때인지 확인하기

private:

	// * TODO : 기술 입력에 제한시간이 있거나 하면 추가

	queue<int> commandQueue; // 입력 큐. FIFO 방식으로 확정

	// -> 이 입력을 하나의 데이터로 모아서 판별, 비교도 해줘야 한다
	// unordered_map<queue<int>, int> commands; // 이쪽이 이론상의 원형

	// -> 실제로는 입력 큐를 게임마다 나름의 방식으로 통합해서 배열화시킨다
	unordered_map<string, int> commandTable; // 입력을 "문자열"로 바꾸고
											 // 문자열에 일치가 있으면
											 // int 지정된 기술을 쓰기

	UINT maxCommandLength = 0;	// 커맨드 길이에 제한을 주고 싶다면
								// * 데이터 전용가능 :
								//      필요하면 "문자열 길이" 등으로 활용가능


};

