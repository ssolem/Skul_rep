#include "Framework.h"
#include "NinjaJumpDown.h"

NinjaJumpDown::NinjaJumpDown(Transform* target)
	: NinjaJump(target)
{
	wstring file = L"Textures/Ninja/Ninja_Jump.png";
	Texture* texture = Texture::Add(file);
	Vector2 cutSize = texture->GetSize() / Vector2(1, 10);

	vector<Frame*> frames;
	// 상승이 [5]까지니까 [6]부터 [9]까지 쓴다
	for (int i = 6; i < 10; ++i)
	{
		frames.push_back(new Frame(
			file, 0, cutSize.y * i, cutSize.x, cutSize.y));
	}

	clip = new Clip(frames, false);
}

void NinjaJumpDown::Update()
{
	NinjaJump::Update();
	
	//if (target->Pos().y <= CENTER_Y)
	//{
	//	target->Pos().y = CENTER_Y;
	//	velocity = 0;
	//	event(); // 이벤트 호출
	//}
}

void NinjaJumpDown::Start()
{
	Action::Start(); // 자체 클립 실행
	velocity = 0; // 중력에 의해 낙하 시작
}
