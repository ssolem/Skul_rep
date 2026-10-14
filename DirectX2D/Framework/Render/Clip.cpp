#include "Framework.h"
#include "Clip.h"

Clip::Clip(vector<Frame*> frames, bool isLoop, float speed, bool ownsFrames)
	: frames(frames), ownsFrames(ownsFrames), isLoop(isLoop), speed(speed)
{
	// 지연 시간 : 해석에 따라 활용도가 높기 때문에 이하 코드는 필요하면 수정

	// -> 샘플 시나리오 : 지연 시간을 '단순 프레임 간격'으로 해석
	delayTime = 1 / FPS;	// 지정된 프레임 다 돌리면 1초.
	frameDelays.assign(frames.size(), delayTime);	// 기본은 모든 프레임 같은 시간
}

Clip::~Clip()
{
	if (!ownsFrames) return;	// 공유 프레임은 캐시가 정리

	for (Frame* frame : frames)
		delete frame;
}

void Clip::Update()
{
	// 재생 중에만 업데이트
	if (!isPlay) return;

	// 재생 중의 업데이트 : 재생 시간을 보면서 맞춘다
	frameTime += speed * DELTA; // 딜레이에 FPS가 계산되어 있어서 여기선 FPS 생략

	// 그 딜레이된, (샘플 시나리오 기준) 프레임 간격 시간에 재생 시간이 도달하면...
	// -> 거꾸로 거기에 도달을 "못 하면"
	if (frameTime < frameDelays[curFrameNum]) return; // 곧바로 루프 종료 (및 재확인)

	// -> 여기까지 왔다면? 간격 시간을 넘은 것
	frameTime -= frameDelays[curFrameNum]; // 시간 리셋 (감소 방식)


	// 재생할 프레임 인덱스를 +1
	curFrameNum++; // 동작이 갱신되면 1차 완성


	// 그 다음 반복 여부를 봐서
	if (isLoop)
	{
		// 프레임 인덱스가 되풀이되도록
		curFrameNum %= frames.size();
	}
	else
	{
		// 끝인지 아닌지만 보면 된다
		if (curFrameNum >= frames.size())
		{
			curFrameNum = frames.size() - 1; // 마지막 인덱스를 주고
			Stop();							 // 멈춘다
		}
	}

	// 지금 프레임이 이벤트 실행 시점으로 등록되었으면 지금 수행
	if (events.count(curFrameNum) > 0)
		events[curFrameNum]();
}

void Clip::Render()
{
	// 갖고 있는 프레임 중에서, 현재 지정 중인 인덱스의 프레임만 렌더하면 그만
	frames[curFrameNum]->Render();
}

void Clip::Play()
{
	// 재생 활성화 + 프레임의 시작을 0프레임으로 + 재생 기록 시간 0

	isPlay = true;
	frameTime = 0.0f;
	curFrameNum = 0;

	if (events.count(0) > 0) events[0]();
}

float Clip::GetDuration() const
{
	float total = 0.0f;
	for (float delay : frameDelays)
		total += delay;

	return total / speed;
}

void Clip::SetFrameDelay(int frameIdx, float seconds)
{
	if (frameIdx < 0 || frameIdx >= frameDelays.size()) return;
	frameDelays[frameIdx] = seconds;
}

void Clip::SetEvent(Event event, int frameIdx)
{
	// 받은 프레임 인덱스가 없거나 (-1) 범위를 벗어났으면
	// "마지막 프레임"이 이벤트 실행 시점인 걸로
	if (frameIdx < 0 || frameIdx >= frames.size())
		frameIdx = frames.size() - 1;

	events[frameIdx] = event;
}
