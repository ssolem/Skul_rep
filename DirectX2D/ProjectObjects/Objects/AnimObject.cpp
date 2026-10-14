#include "Framework.h"
#include "AnimObject.h"
#include "ProjectObjects/Character/Animator.h"

AnimObject::AnimObject()
{
	animator = new Animator();
	Scale() = SKUL_DEFAULT_ASSET_SIZE;
	SetActive(false);
}

AnimObject::~AnimObject()
{
	delete animator;
}

void AnimObject::Update()
{
	if (!Active()) return;

	UpdateWorld();
	animator->Update();		
}

void AnimObject::Render()
{
	if (!Active()) return;

	SetRender();
	animator->Render();
}

void AnimObject::AddClip(const string& fileName, const wstring& folder, bool isLoop, float speed)
{
	animator->CreateClipByAtlas("", fileName, folder, isLoop, speed);
}

Clip* AnimObject::FindClip(const string& key)
{
	return animator->FindClip(key);
}

void AnimObject::Play(const string& key, Vector2 pos)
{
	Pos() = pos;
	SetActive(true);
	UpdateWorld();

	animator->PlayClip(key);
}
