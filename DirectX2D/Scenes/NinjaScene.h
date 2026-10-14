#pragma once

class Ninja;

class NinjaScene : public Scene
{
public:
	NinjaScene();
	~NinjaScene();

	virtual void Update() override;
	virtual void Render() override;
	virtual void PostRender() override;

private:

	Ninja* ninja;

};

