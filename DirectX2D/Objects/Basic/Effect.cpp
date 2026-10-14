#include "Framework.h"

Effect::Effect(vector<Frame*> frames, float speed, bool isAdditive)
	: frames(frames), speed(speed), isAdditive(isAdditive) // 변수 초기화
{
	//PixelUV는 color를 곱하지 않으므로 SetColor가 적용되도록 Effect 전용 셰이더 사용
	SetPixelShader(L"Effect.hlsl");

	colorBuffer = new ColorBuffer();

	isActive = false; // 실제 재생하기 전까지는 보이지 않게 (풀링 방식)

	frameDelays.assign(frames.size(), 1 / FPS);
}

Effect::~Effect()
{
	delete colorBuffer;
	//frames는 빌린 것이라 지우지 않음
}

void Effect::Update()
{
	// 재생 중일때 반드시 켜고
	if (!isActive) return;

	frameTime += speed * DELTA;

	if (frameTime >= frameDelays[curFrameNum]) // 현재 프레임 표시 시간이 지나면
	{
		// 다음순서의 애니메이션 재생 처리
		frameTime -= frameDelays[curFrameNum];
		curFrameNum++;

		if (curFrameNum >= frames.size()) // 실제 프레임 수에서 종료 - 빈 칸 재생 없음
		{
			End();
			return;
		}
	}
	UpdateWorld();
}

void Effect::Render()
{
	if (!isActive) return;

	if (isAdditive) // 화면 합산 (밝게)
		Environment::Get()->SetAdditiveBlend(); // 밝게만 그리기 -> 화면 혼합에서 합산해 보이도록

	colorBuffer->SetPS(0);
	SetRender();
	frames[curFrameNum]->Render();

	Environment::Get()->SetAlphaBlend(); // 기본값 그리기 -> 렌더링 알파 혼합로 되돌림
}

void Effect::Play(Vector2 position)
{
	isActive = true;
	Pos() = position;

	frameTime = 0;
	curFrameNum = 0;

	UpdateWorld(); // 첫 프레임이 이전 위치에 한 번 그려지는 것 방지
}

void Effect::End()
{
	isActive = false;

	if (onEnd)
	{
		function<void()> callback = onEnd;
		onEnd = nullptr;
		callback();
	}
}

void Effect::SetFrameDelay(UINT frame, float delay)
{
	if (frame >= frames.size()) return;
	frameDelays[frame] = delay;
}

void Effect::CopySettings(Effect* source)
{
	Scale() = source->Scale();
	GetColor() = source->GetColor();
	frameDelays = source->frameDelays;
}
