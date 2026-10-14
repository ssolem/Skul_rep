#pragma once
class LineScene : public Scene
{
public:
	LineScene();
	~LineScene();

	virtual void Update() override;
	virtual void Render() override;
	virtual void PostRender() override;

private:

	LineCollider* line;
};

