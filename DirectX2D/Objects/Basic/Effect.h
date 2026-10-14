#pragma once

class Effect : public Quad
{
private:
	const float FPS = 10; // 이펙트 재생용 기본 FPS

	// 이펙트 프레임 정보용 버퍼 (셰이더에 "총 프레임수, 현재 프레임"을 넘길 때 씀)
	class FrameBuffer : public ConstBuffer
	{
	private:
		struct Data
		{
			Vector2 maxFrame;	// 이 이펙트의 총 프레임개수
			Vector2 curFrame;	// 현재 몇 번째 프레임인지
		};

	public:
		FrameBuffer() : ConstBuffer(&data, sizeof(Data)) {}
		void SetFrame(Vector2 maxFrame) { data.maxFrame = maxFrame; }
		Vector2& GetCurFrame() { return data.curFrame; }
	private:
		Data data;
	};

public:
	Effect(
		wstring textureFile,	// 텍스처파일
		Vector2 maxFrame,		// 최대 프레임수 (-> 최대 프레임)
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

private:

	FrameBuffer* frameBuffer; // 셰이더 전달용 + 현재 몇 번

	// 이펙트는, 2D 월드에서 부딪히는 이벤트 없이 순수 연출용으로만 쓰는, 별도의 클래스

	float delayTime = 1 / FPS; // 프레임간 재생 간격
	float frameTime = 0;	   // 재생 시간
	float speed;			   // 재생 속도(배율)
	vector<float> frameDelays;

	UINT curFrameNum = 0;	   // 클립에서 재생하는 프레임 현재 순서
	UINT maxFrameNum = 0;

	UINT maxFrameX = 0;		   // 가로

	bool isAdditive;		   // RGB 합산

	function<void()> onEnd;
};
