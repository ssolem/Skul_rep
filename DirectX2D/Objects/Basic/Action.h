#pragma once
class Action
{
public:

	Action(string path, string file, bool isLoop, float speed = 1);
	~Action();

	virtual void Update();
	virtual void Render();
	
	virtual void Start();
	virtual void End();

	Clip* GetClip() { return clip; }
	void SetEvent(Event event) { this->event = event; }

protected:
	Clip* clip;
	Event event = nullptr; // 도중 혹은 다음에 일어날 일
						   // -> 상용 엔진이나 3D 애니메이션으로 치면
						   //   "모션 이벤트" "애님 노티파이"
	
	void Call_LoadClip(string path, string file, bool isLoop, float speed)
	{
		LoadClip(path, file, isLoop, speed);
	}

private:
	void LoadClip(string path, string file, bool isLoop, float speed);

};

