#include "Framework.h"
#include "Projectile.h"
#include "ProjectObjects/Character/Animator.h"
#include "ProjectObjects/Character/Character.h"

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

	Pos() += moveDir * data.speed * DELTA;
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
	animator->CreateClipByAtlas(data.tag, "Projectile", data.path);
	animator->CreateClipByAtlas(data.tag, "Projectile_Hit", data.path, false);

	//Hit 클립이 끝나면 풀로 반환 - 클립은 투사체마다 따로라 this 캡처 안전 (다시 걸어도 덮어씀)
	Clip* hitClip = animator->FindClip(data.tag + "Projectile_Hit");
	if (hitClip)
		hitClip->SetEvent([this]() { Reset(); });

	//size가 0이면 Projectile 클립 첫 프레임 크기 사용
	Clip* flyClip = animator->FindClip(data.tag + "Projectile");
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
	lifeTime = 0.0f;
	moveDir = Vector2::Zero;
	SetActive(false);
}

void Projectile::Fire(Vector2 pos, Vector2 dir, const HitData& hit, ProjectileTarget target)
{
	moveDir = dir.GetNormalized();
	hitData = hit;
	this->target = target;
	lifeTime = data.lifeTime;
	isFlying = true;
	isDestroying = false;
	canHit = true;		//풀 재사용 시 초기화
	groupId = 0;

	bool flip = moveDir.x < 0.0f;
	Pos() = pos;
	Scale() = { SKUL_DEFAULT_ASSET_SIZE.x * (flip ? -1.0f : 1.0f), SKUL_DEFAULT_ASSET_SIZE.y };

	float theta = atan2f(moveDir.y, fabsf(moveDir.x));
	Rot().z = flip ? -theta : theta;

	animator->PlayClip(data.tag + "Projectile");

	UpdateWorld();
	collider->UpdateWorld();
	SetActive(true);
}

void Projectile::Destroy()
{
	if (isDestroying) return;

	//Hit 클립이 없으면 바로 반환
	if (animator->FindClip(data.tag + "Projectile_Hit") == nullptr)
	{
		Reset();
		return;
	}

	//멈추고 Hit 클립 재생 → 클립 끝 이벤트에서 Reset()
	isDestroying = true;
	animator->PlayClip(data.tag + "Projectile_Hit");
}

void Projectile::Hit(Character* character)
{
	if (!data.pierce) isFlying = false;

	hitData.attackDir = GlobalPos().x < character->GlobalPos().x ? 1.0f : -1.0f;
	character->OnHit(hitData);
	Destroy();
}
