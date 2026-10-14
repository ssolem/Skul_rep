#pragma once

class Clip
{
private:
	// 클립 재생 계수에 영향을 주는 실질 재생 속도
	const float FPS = 10;

public:
	// 클립 : 프레임들을 모아 하나의 동작으로 만든 것
	// ownsFrames = false : 프레임을 공유 캐시에서 빌려 씀 (소멸 시 삭제하지 않음)
	Clip(vector<Frame*> frames, bool isLoop = true, float speed = 1, bool ownsFrames = true);
	~Clip();

	void Update();
	void Render();

	void Play();
	void Stop() { isPlay = false; } // 재생 중단
	void SetLoop(bool value) { isLoop = value; } // 반복여부 설정
	void SetSpeed(float value) { speed = value; }

	Frame* GetCurFrame() { return frames[curFrameNum]; }
	Frame* GetFrame(int frameNum)
	{
		if (frameNum >= frames.size() || frameNum < 0) return nullptr;
		return frames[frameNum];
	}
	int GetFrameCount() { return frames.size(); }

	float GetFrameInterval() const { return delayTime / speed; }              // 기본 프레임 1칸 시간
	float GetDuration() const;                                                 // 전체 재생 시간 (프레임별 시간 합)

	void SetFrameDelay(int frameIdx, float seconds);	// 해당 프레임만 표시 시간 변경 (speed는 그대로 곱해짐)

	void SetEvent(Event event, int frameIdx = -1);

private:
	vector<Frame*> frames;
	bool ownsFrames = true;	// 프레임 소유 여부
	UINT curFrameNum = 0; // 현재 노출 중인 프레임

	float frameTime = 0;	// 프레임 재생 시간
	float speed = 0;		// 속도 계수
	float delayTime = 0;	// 지연 시간
	vector<float> frameDelays;	// 프레임별 표시 시간 - 생성자에서 전부 delayTime으로 채움

	bool isLoop = false;	// 반복 여부
	bool isPlay = false;	// 재생 여부

	map<int, Event> events; // 프레임별 이벤트
};

