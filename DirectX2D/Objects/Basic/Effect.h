#pragma once

// 이펙트는, 2D 월드에서 부딪히는 이벤트 없이 순수 연출용으로만 쓰는, 별도의 클래스
// 풀링되는 1회 재생 애니메이션 - 프레임은 빌려 씀 (소유: Animator 프레임 캐시 또는 EffectManager)
class Effect : public GameObject
{
private:
	const float FPS = 10; // 이펙트 재생용 기본 FPS

public:
	Effect(
		vector<Frame*> frames,	// 재생할 프레임 목록 (.uv 아틀라스 또는 균등 격자로 만든 것)
		float speed,			// 재생 속도
		bool isAdditive			// RGB 화면 간 합산 여부
	);
	~Effect();

	void Update();
	void Render();

	void Play(Vector2 position);
	void End();

	void OnEnd(function<void()> callback) { onEnd = callback; }
	void SetFrameDelay(UINT frame, float delay);
	void CopySettings(Effect* source);	//풀 확장 시 기존 이펙트의 크기/색/프레임 시간 복사

	Float4& GetColor() { return colorBuffer->Get(); }

private:
	vector<Frame*> frames;	   // 빌린 프레임 - 지우지 않음
	ColorBuffer* colorBuffer;  // Frame에는 색 버퍼가 없어서 Effect가 직접 가짐

	float frameTime = 0;	   // 현재 프레임 경과 시간
	float speed;			   // 재생 속도(배율)
	vector<float> frameDelays; // 프레임별 표시 시간 - 기본 1 / FPS

	UINT curFrameNum = 0;	   // 현재 재생 중인 프레임 순서

	bool isAdditive;		   // RGB 합산

	function<void()> onEnd;
};
