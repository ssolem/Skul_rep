#include "Framework.h"

Effect::Effect(wstring textureFile, Vector2 maxFrame, float speed, bool isAdditive)
	: Quad(textureFile, Vector2(), // 그림 파일과 시작 UV
		Vector2(1 / maxFrame.x, 1 / maxFrame.y)), // 끝 UV
	speed(speed), isAdditive(isAdditive) // 변수 초기화
{
	pixelShader = Shader::AddPS(L"PixelEffect.hlsl"); // 화면합산용

	isActive = false; // 실제 재생하기 전까지는 보이지 않게 (풀링 방식)

	frameBuffer = new FrameBuffer(); // 셰이더 전달용 생성
	frameBuffer->SetFrame(maxFrame);

	maxFrameNum = (UINT)(maxFrame.x * maxFrame.y);
	maxFrameX = (UINT)maxFrame.x;

	frameDelays.assign(maxFrameNum, delayTime);
}

Effect::~Effect()
{
	delete frameBuffer;
}

void Effect::Update()
{
	// 재생 중일때 반드시 켜고
	if (!isActive) return;

	frameTime += speed * DELTA;

	if (frameTime >= delayTime) // 시간 간격이 되면
	{
		// 다음순서의 애니메이션 재생 처리
		frameTime -= frameDelays[curFrameNum];
		curFrameNum++;

		frameBuffer->GetCurFrame().x = (float)(curFrameNum % maxFrameX);
		frameBuffer->GetCurFrame().y = (float)(curFrameNum / maxFrameX);

		if (curFrameNum >= maxFrameNum) End(); // 마지막 재생 종료처리
	}
	UpdateWorld();
}

void Effect::Render()
{
	if (!isActive) return;

	if (isAdditive) // 화면 합산 (밝게)
		Environment::Get()->SetAdditiveBlend(); // 밝게만 그리기 -> 화면 혼합에서 합산해 보이도록

	frameBuffer->SetPS(1);
	Quad::Render();

	Environment::Get()->SetAlphaBlend(); // 기본값 그리기 -> 렌더링 알파 혼합로 되돌림

}

void Effect::Play(Vector2 position)
{
	isActive = true;
	Pos() = position;

	frameTime = 0;
	curFrameNum = 0;
	frameBuffer->GetCurFrame() = { 0, 0 };
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
	if (frame >= maxFrameNum) return;
	frameDelays[frame] = delay;
}

void Effect::CopySettings(Effect* source)
{
	Scale() = source->Scale();
	GetColor() = source->GetColor();
	frameDelays = source->frameDelays;
}
