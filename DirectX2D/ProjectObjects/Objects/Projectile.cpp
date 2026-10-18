#include "Framework.h"
#include "Projectile.h"
#include "ProjectObjects/Character/Animator.h"
#include "ProjectObjects/Character/Character.h"
#include "ProjectObjects/Stage.h"

Projectile::Projectile()
{
	animator = new Animator();

	collider = new RectCollider(Vector2::One);
	collider->SetParent(this);

	SetActive(false);
}

Projectile::~Projectile()
{
	delete animator;
	delete collider;
}

void Projectile::Update()
{
	if (!Active()) return;

	animator->Update();

	if (isDestroying) return;	//Hit 클립 재생 중에는 이동 안 함

	lifeTime -= DELTA;
	if (lifeTime <= 0.0f)
	{
		Destroy();
		return;
	}

	if (!isStuck)
	{
		velocity.y -= data.gravity * DELTA;
		Pos() += velocity * DELTA;
		UpdateRotation();
	}

	UpdateWorld();
	collider->UpdateWorld();
}

void Projectile::Render()
{
	if (!Active()) return;

	SetRender();
	animator->Render();
	collider->Render();
}

void Projectile::Init(const ProjectileData& data)
{
	this->data = data;

	//클립은 이 투사체의 animator에 생성 - 이미 있으면 내부에서 건너뜀
	animator->CreateClipByAtlas(data.tag, data.flyClip, data.path);
	if (!data.hitClip.empty())
		animator->CreateClipByAtlas(data.tag, data.hitClip, data.path, false);

	//Hit 클립이 끝나면 풀로 반환 - 클립은 투사체마다 따로라 this 캡처 안전 (다시 걸어도 덮어씀)
	Clip* hitClip = FindHitClip();
	if (hitClip)
		hitClip->SetEvent([this]() { Reset(); });

	//size가 0이면 비행 클립 첫 프레임 크기 사용
	Clip* flyClip = animator->FindClip(data.tag + data.flyClip);
	if (this->data.size == Vector2::Zero && flyClip)
		this->data.size = flyClip->GetFrame(0)->GetSize();

	//콜라이더는 자식이라 부모 스케일이 곱해짐 → 프레임 크기를 그대로 넣으면 스프라이트와 같은 크기
	collider->Scale() = this->data.size;
	collider->Pos().y = this->data.size.y * 0.5f;
}

void Projectile::Reset()
{
	isFlying = false;
	isDestroying = false;
	isStuck = false;
	lifeTime = 0.0f;
	velocity = Vector2::Zero;
	alreadyHit.clear();
	onHit = nullptr;
	SetActive(false);
}

void Projectile::Fire(Vector2 pos, Vector2 dir, const HitData& hit, ProjectileTarget target, OnHitEvent onHit, int maxHitCount)
{
	//발사마다 관통 수를 바꾸는 스킬용 (단계별 관통 등) - Init이 매번 등록값으로 되돌림
	if (maxHitCount != 0)
		data.maxHitCount = maxHitCount;

	Vector2 moveDir = dir.GetNormalized();
	velocity = moveDir * data.speed;
	hitData = hit;
	this->target = target;
	this->onHit = onHit;
	lifeTime = data.lifeTime;
	isFlying = true;
	isDestroying = false;
	isStuck = false;
	alreadyHit.clear();
	canHit = true;		//풀 재사용 시 초기화
	groupId = 0;

	bool flip = moveDir.x < 0.0f;
	Pos() = pos;
	Scale() = { SKUL_DEFAULT_ASSET_SIZE.x * (flip ? -1.0f : 1.0f), SKUL_DEFAULT_ASSET_SIZE.y };

	float theta = atan2f(moveDir.y, fabsf(moveDir.x));
	Rot().z = flip ? -theta : theta;

	animator->PlayClip(data.tag + data.flyClip);

	UpdateWorld();
	collider->UpdateWorld();
	SetActive(true);
}

void Projectile::Destroy()
{
	if (isDestroying) return;

	//Hit 클립이 없으면 바로 반환
	if (FindHitClip() == nullptr)
	{
		Reset();
		return;
	}

	//멈추고 Hit 클립 재생 → 클립 끝 이벤트에서 Reset()
	isDestroying = true;
	animator->PlayClip(data.tag + data.hitClip);
}

Clip* Projectile::FindHitClip()
{
	//hitClip이 비어 있으면 tag만으로 찾게 되므로 미리 거름

	if (data.hitClip.empty()) return nullptr;

	return animator->FindClip(data.tag + data.hitClip);
}

void Projectile::Hit(Character* character)
{
	if (find(alreadyHit.begin(), alreadyHit.end(), character) != alreadyHit.end()) return;
	alreadyHit.push_back(character);

	hitData.attackDir = GlobalPos().x < character->GlobalPos().x ? 1.0f : -1.0f;
	character->OnHit(hitData);
	if (onHit) onHit(character);

	//아직 더 맞출 수 있으면 계속 날아가거나 박힌 채로 남음

	if (data.maxHitCount < 0 || (int)alreadyHit.size() < data.maxHitCount) return;

	isFlying = false;
	Destroy();
}

void Projectile::CheckTileCollision(Stage* stage)
{
	if (!Active() || !isFlying || isStuck || isDestroying) return;
	if (data.tileHitType == TileHitType::Pass) return;

	Tile* tile = stage->CheckWallTileCollision(collider);

	//바닥/계단은 내려가는 중일 때만 (발판을 아래에서 뚫고 올라가거나 수평으로 지나갈 때는 무시)

	bool hitFloor = false;
	if (tile == nullptr && velocity.y < 0.0f)
	{
		tile = stage->CheckFloorTileCollision(collider);
		if (tile == nullptr)
			tile = stage->CheckStairTileCollision(collider);
		hitFloor = tile != nullptr;
	}

	if (tile == nullptr) return;

	if (data.tileHitType == TileHitType::Destroy)
	{
		isFlying = false;
		Destroy();
		return;
	}

	//Stick: 그 자리에 멈추고 lifeTime 동안 판정 유지

	isStuck = true;
	velocity = Vector2::Zero;

	if (hitFloor)
	{
		Pos().y = tile->GetCollider()->T();	//원점이 투사체 바닥이라 타일 윗면에 맞춤
		UpdateWorld();
		collider->UpdateWorld();
	}
}

void Projectile::UpdateRotation()
{
	if (!data.rotateToVelocity) return;
	if (velocity.Length() < FLT_EPSILON) return;

	//좌우 반전은 Scale.x가 담당하므로 각도는 항상 오른쪽 기준으로 계산

	float theta = atan2f(velocity.y, fabsf(velocity.x));
	Rot().z = velocity.x < 0.0f ? -theta : theta;
}
