#pragma once
class ButtonScene : public Scene
{
public:
	ButtonScene();
	~ButtonScene();

	virtual void Update() override;
	virtual void Render() override;
	virtual void PostRender() override;

private:

	// Button* button;

	vector<wstring> buttonKeys;	// 버튼 생성에 일괄대입할 파일명들
	vector<ButtonObject*> buttons;

	UINT size = 16;
	UINT width = 4;	// 한 줄에 몇 개 나올 건지
	UINT height = 4;

};

