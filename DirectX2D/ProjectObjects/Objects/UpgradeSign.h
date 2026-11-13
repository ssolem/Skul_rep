#pragma once
class AnimObject;

class UpgradeSign
{
public:
	UpgradeSign();
	~UpgradeSign();

	void Update();
	void Render();

	void Show(Vector2 groundPos);
	void Hide();

	bool IsInteractable(Collider* player);

private:
	const Vector2 COL_OFFSET = Vector2(0.0f, 16.0f);
	const Vector2 COL_SIZE = Vector2(30.0f, 32.0f);

	Transform* position = nullptr;
	AnimObject* sign = nullptr;
	RectCollider* collider = nullptr;
};
