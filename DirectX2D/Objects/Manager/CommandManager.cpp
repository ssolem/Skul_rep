#include "Framework.h"

CommandManager::CommandManager()
{
	// 일단은 닌자용 액션을 int로 지정 : 상태의 enum에서 값을 가져오기
	// 10 : 지상공격, 11 : 점프공격

	// 커맨드 테이블은 커맨드가 string 키, 기술이 int 값

	// -> 기술을 화살표만으로 작성하면 오작동 문제가 있어 완결 키를 추가
	commandTable["26A"] = 10; // 아래 앞 공격
	commandTable["66A"] = 11; // 앞 앞 공격

	for (pair<string, int> command : commandTable)
	{
		// 최대 커맨드 길이를 변수명대로 쓸 거라면 그 길이가
		// 기술 길이 중 가장 긴 것보다 짧아서는 안 된다
		if (command.first.size() > maxCommandLength)
			maxCommandLength = command.first.size();
	}

}
void CommandManager::Update()
{
	// 상시관리 버전 : 키 입력이 있으면 커맨드 추가 호출

	// * 현재 키를 string으로 관리하려는 중
	//   그리고 방향키는 키패드 키에 대응시켜 문자화하려는 것
	//   각 키를 의미하는 '숫자'가 무엇이냐가 중요한데

	if (KEY_DOWN(VK_RIGHT))
		PushCommand('0' + 6); // '0'에 해당하는 문자 코드에서 6번째 뒤 == 6

	if (KEY_DOWN(VK_LEFT))
		PushCommand('0' + 4);

	if (KEY_DOWN(VK_UP))
		PushCommand('0' + 8);

	if (KEY_DOWN(VK_DOWN))
		PushCommand('0' + 2);

	if (KEY_DOWN('S')) // 일부러 다른 키로 커맨드 완결 (테스트 및 중복방지)
	{
		PushCommand('A'); // 완결키
		CheckCommand();		  // 키 비교
	}

	// TODO #1 : 커맨드 자동소멸은 어떻게 할 수 있을까...?
	//           커맨드 연결이 너무 오래 되면 리셋되게

	// TODO #2 : 닌자가 왼쪽을 보고 있었더라면 기술을 어떻게 하면 쓸 수 있을까?
	//       A : "왼쪽용 커맨드가 필요할까?"
	//       B : "똑같은 오른쪽을 누르는데 캐릭터 방향에 따른 의미는 어떻게
	//            다르게 할 수 있을까?
}

void CommandManager::GetInput(int key)
{
	//TODO : 호출되면 받은 키를 커맨드 추가 호출에 전달
	PushCommand(key); // <- 필수항목
}

void CommandManager::PushCommand(int key)
{
	commandQueue.push(key);

	if (commandQueue.size() > maxCommandLength)
		commandQueue.pop(); // 허용받은 입력 길이를 초과하면 먼저 받은 키를 팝

	// -> 복잡한 키가 들어왔을 때도 앞에 입력한 잘못된 키를 제외시키고
	//    필요한 만큼만 자료 비교를 하는 것이 가능
}

void CommandManager::CheckCommand()
{
	string command; // 받은 커맨드를 문자열로 받기 위한 변수

	// 받은 입력 큐를 문자열로

	while (commandQueue.size() > 0) // 입력 큐가 남은 동안 진행
	{
		command += commandQueue.front(); // int 추가를 통한 문자 append
		commandQueue.pop();
	}

	// 여기까지 오면 문자열 준비 : 다음은 맵에서 기술 찾기
	while (command.size() > 0) // 문자열이 성립하면
	{
		if (commandTable.count(command) > 0) // 맵에 기술이 있으면
		{
			// TODO : 기술 사용자가 기술에 대한 신호를 받도록
			//        1. 이 클래스가 변수 수정으로 감지 가능한 변화를 주거나
			//        2. 이럴 때 쓰는 것이 이벤트...인데 여기가 전역 클래스니까
			//           이런 경우라면 이벤트도 전역에서 보면 좋을 것이다.

			Observer::Get()->ExecuteIntParamEvent(
				"Action",	// 호출할 함수의 키 이름. 이 이름으로 닌자도 함수 등록
				commandTable[command] // 인수는 맵 속의 커맨드의 대응 값
			);
		}

		// 최대 길이보다 '짧은' 기술이 사용됐을 경우
		// "이동" 입력 부분이 조건에 맞지 않는 경우가 있을 수 있다

		// 예 : 커맨드 길이를 10까지 상황에서
		//     장풍(236A)을 쓰기 위해 커맨드 "41236A"를 입력한 상황

		// 그래서 "앞부분의 선입력"을 빼고도 성립하는 다른 커맨드가 있는지
		// 한 번 더 확인해볼 필요가 있다

		command.erase(command.begin()); // 맨 앞의 자료를 pop
	}
}