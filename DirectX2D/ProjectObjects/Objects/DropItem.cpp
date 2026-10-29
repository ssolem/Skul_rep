#include "Framework.h"
#include "DropItem.h"
#include "ProjectObjects/Stage.h"
#include "ProjectObjects/Character/Character.h"
#include "ProjectObjects/Character/Animator.h"

//아틀라스별 다음 낱장 번호 - 랜덤 대신 순서대로 돌려서 고르게 나오게
static unordered_map<const vector<Frame*>*, UINT> imageCounters;

DropItem::DropItem()
{
	Scale() = SKUL_DEFAULT_ASSET_SIZE;
	SetActive(false);
}

DropItem::~DropItem()
{
	delete idleClip;
	delete ownedImage;
	delete collider;
}

void DropItem::Update()
{
	if (!Active()) return;

	lifeTime += DELTA;

	if (state == State::Drop)
	{
		if (isArc) UpdateArc();
		else UpdateDrop();
	}

	//시간이 지나면 떨어지는 중이어도 그 자리에서 획득
	if (autoAcquire && lifeTime >= acquireDelay)
	{
		Acquire();
		return;
	}

	UpdateWorld();
	collider->UpdateWorld();
	if (idleClip) idleClip->Update();
}

void DropItem::Render()
{
	if (!Active()) return;

	SetRender();
	if (idleClip) idleClip->Render();
	else if (image) image->Render();
	collider->Render();
}

void DropItem::Spawn(Stage* stage, Vector2 pos)
{
	this->stage = stage;

	//위쪽 랜덤 방향, 힘도 랜덤
	float angle = XMConvertToRadians(90.0f + Random(-spreadAngle, spreadAngle));
	float power = Random(minPower, maxPower);
	velocity = Vector2(cos(angle), sin(angle)) * power;

	state = State::Drop;
	isArc = false;
	bounceCount = 0;
	lifeTime = 0.0f;

	if (images)
	{
		image = PickImage();
		collider->SetSize(image->GetSize());
	}

	Pos() = pos;
	SetActive(true);
	UpdateWorld();
	collider->UpdateWorld();
	prevBottom = collider->B();

	if (idleClip) idleClip->Play();
}

void DropItem::SpawnTo(Stage* stage, Vector2 start, Vector2 target)
{
	Spawn(stage, start);

	float bottomOffset = GlobalPos().y - collider->B();
	arcStart = start;
	arcTarget = stage->GetGroundPos(target) + Vector2(0.0f, bottomOffset);

	float y = max(arcStart.y, arcTarget.y) + arcHeight;
	float riseTime = sqrt(2.0f * (y - arcStart.y) / GRAVITY);
	float fallTime = sqrt(2.0f * (y - arcTarget.y) / GRAVITY);
	arcTime = riseTime + fallTime;

	arcVelocity.x = (arcTarget.x - arcStart.x) / arcTime;
	arcVelocity.y = GRAVITY * riseTime;

	isArc = true;
}

void DropItem::SetIdleClip(const wstring& atlasPath)
{
	vector<Frame*>* frames = Animator::GetAtlasFrames(atlasPath, true);
	if (frames == nullptr) return;

	idleClip = new Clip(*frames, true, 1.0f, false);
	CreateCollider(*frames);
}

void DropItem::SetImages(const wstring& atlasPath)
{
	images = Animator::GetAtlasFrames(atlasPath, true);
	if (images == nullptr || images->empty())
	{
		images = nullptr;
		return;
	}

	CreateCollider(*images);
}

void DropItem::SetAcquireEffect(const string& key, const wstring& atlasPath, float speed)
{
	acquireEffectKey = key;
	if (EffectManager::Get()->HasEffect(key)) return;

	vector<Frame*>* frames = Animator::GetAtlasFrames(atlasPath, true);
	if (frames == nullptr) return;

	EffectManager::Get()->Add(key, 4, *frames, speed);
	EffectManager::Get()->SetScale(key, SKUL_DEFAULT_ASSET_SIZE);
}

void DropItem::SetImage(const wstring& texturePath)
{
	delete ownedImage;
	ownedImage = new Frame(texturePath);
	image = ownedImage;

	//이미지 크기로 콜라이더 갱신
	if (collider == nullptr)
	{
		collider = new RectCollider(image->GetSize());
		collider->SetParent(this);
	}
	else
	{
		collider->SetSize(image->GetSize());
	}
}

void DropItem::CreateCollider(const vector<Frame*>& frames)
{
	Vector2 size;
	for (Frame* frame : frames)
	{
		size.x = max(size.x, frame->GetSize().x);
		size.y = max(size.y, frame->GetSize().y);
	}

	collider = new RectCollider(size);
	collider->SetParent(this);
}

Frame* DropItem::PickImage()
{
	UINT& counter = imageCounters[images];
	Frame* picked = (*images)[counter % images->size()];
	counter++;

	return picked;
}

void DropItem::UpdateDrop()
{
	prevBottom = collider->B();

	velocity.y -= GRAVITY * DELTA;
	Pos() += velocity * DELTA;
	UpdateWorld();
	collider->UpdateWorld();

	ResolveTileCollision();
}

void DropItem::UpdateArc()
{
	float t = min(lifeTime, arcTime);
	Pos().x = arcStart.x + arcVelocity.x * t;
	Pos().y = arcStart.y + arcVelocity.y * t - 0.5f * GRAVITY * t * t;

	if (lifeTime >= arcTime)
	{
		Pos() = arcTarget;
		velocity = Vector2::Zero;
		state = State::Rest;
	}
}

void DropItem::Acquire()
{
	if (!acquireEffectKey.empty())
		EffectManager::Get()->Play(acquireEffectKey, GlobalPos());	//아이템 위치에서 재생

	//OnAcquire에서 다시 Spawn할 수 있도록 먼저 끔
	SetActive(false);
	OnAcquire(stage->GetPlayer());
}

void DropItem::ResolveTileCollision()
{
	Vector2 overlap;

	//벽/바닥 타일 - 사방으로 막힘
	if (Tile* wall = stage->CheckWallTileCollision(collider, &overlap))
		PushOut(wall, overlap);

	if (state != State::Drop) return;

	if (Tile* floor = stage->CheckFloorTileCollision(collider, &overlap))
		PushOut(floor, overlap);

	if (state != State::Drop) return;

	//위에서 떨어질 때만 착지, 옆/아래에서는 통과
	if (velocity.y > 0.0f) return;

	if (Tile* stair = stage->CheckStairTileCollision(collider, &overlap))
	{
		float tileTop = stair->GetCollider()->T();

		if (prevBottom >= tileTop - 6.0f && collider->B() < tileTop)
		{
			Pos().y += tileTop - collider->B();
			UpdateWorld();
			collider->UpdateWorld();

			Land();
		}
	}
}

void DropItem::PushOut(Tile* tile, Vector2 overlap)
{
	Vector2 tilePos = tile->GetCollider()->GlobalPos();

	if (overlap.x < overlap.y)
	{
		//옆면 - 타일 반대쪽으로 밀고 가로 속도 반사
		float pushDir = GlobalPos().x < tilePos.x ? -1.0f : 1.0f;
		Pos().x += pushDir * overlap.x;
		velocity.x = pushDir * abs(velocity.x) * wallBounceRate;
	}
	else if (GlobalPos().y < tilePos.y)
	{
		//천장 - 아래로 밀고 올라가던 힘 없앰
		Pos().y -= overlap.y;
		velocity.y = min(velocity.y, 0.0f);
	}
	else
	{
		//윗면 - 착지
		Pos().y += overlap.y;
		Land();
	}

	UpdateWorld();
	collider->UpdateWorld();
}

void DropItem::Land()
{
	if (velocity.y > 0.0f) return;	

	bounceCount++;

	if (bounceCount >= maxBounce)
	{
		velocity = Vector2::Zero;
		state = State::Rest;
		return;
	}

	velocity.y = -velocity.y * bounceRate;
	velocity.x *= groundFriction;
}
