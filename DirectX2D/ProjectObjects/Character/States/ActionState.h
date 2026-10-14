#pragma once
#include "../Character.h"

class ActionState
{
public:

	ActionState(Character* character, ActionType type, const vector<string>& clipNames);
	virtual ~ActionState();

	virtual void Update();
	virtual void UpdateCoolDown();
	virtual void Render();
	virtual void PlayClip();

	virtual void Start();
	virtual void End();

	virtual bool IsReady() { return true; }

	void ResetActions(const vector<string>& clips);

	Character* GetOwner() { return owner; }
	ActionType GetType() { return type; }
	void SetEvent(Event event) { this->event = event; }
	const string& GetClipName() { return curClipName; }

protected:
	Character* owner;
	ActionType type;
	string curClipName;
	vector<string> clipNames;

	//Clip* clip;
	Event event = nullptr;

private:

};

