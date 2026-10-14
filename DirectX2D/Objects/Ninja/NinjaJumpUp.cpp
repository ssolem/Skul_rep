#include "Framework.h"
#include "NinjaJumpUp.h"

NinjaJumpUp::NinjaJumpUp(Transform* target) : NinjaJump(target)
{
	// 점프 상승의 특징 -> 전체 클립을 쓰지 않음 : 상승만 뺀다
	// 발이 모여서 상승처럼 보이는 프레임 [5]까지 쓰기로 하고...

	wstring file = L"Textures/Ninja/Ninja_Jump.png";
	Texture* texture = Texture::Add(file); // 어차피 스태틱. 확인사살
	Vector2 cutSize = texture->GetSize() / Vector2(1, 10);

	vector<Frame*> frames;
	for (int i = 0; i < 6; ++i)
	{
		frames.push_back(new Frame(
			file, 0, cutSize.y * i, cutSize.x, cutSize.y));
	}
	
	clip = new Clip(frames, false);
}

void NinjaJumpUp::Update()
{
	NinjaJump::Update(); // 점프 상승, 하강, 혹은 진행 중 가로 이동

	if (velocity <= 0)
	{
		event(); // 세팅된 이벤트 (아마 하강) 실행
	}

}

void NinjaJumpUp::Start()
{
	Action::Start(); // 가지고 있는 클립 포함 액션 실행
	velocity = JUMP_POWER; // Y속력 초기화
}
