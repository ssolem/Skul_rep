#include "Framework.h"
#include "Character.h"
#include "Animator.h"
#include "ProjectObjects/Stage.h"
#include "Stat.h"


Character::Character()
{
	Init();

	animator = new Animator();
}

Character::~Character()
{
	delete animator;
	delete collider;
	delete groundSensor;
	delete attackCollider;
	delete stat;
}

// 애니메이션 클립이 준비된 뒤 파생 클래스가 호출한다 (defaultSize 가 클립 크기에 의존)
void Character::InitBody()
{
	defaultSize = animator->GetCurFrameSize();

	collider = new RectCollider(defaultSize);
	collider->SetParent(this);
	collider->SetTag("BodyCollider");

	groundSensor = new RectCollider(defaultSize);
	groundSensor->SetParent(this);
	groundSensor->SetTag("GroundSensor");

	attackCollider = new RectCollider(Vector2::Zero);
	attackCollider->SetParent(this);
	attackCollider->SetTag("AttackCollider");
	attackCollider->SetActive(false);

	prevBottom = ((RectCollider*)collider)->B();
}

void Character::Init()
{
	Scale() = SKUL_DEFAULT_ASSET_SIZE;
	
	stat = new Stat();

	ResetMove();
	isPlayingAction = false;
	isGrounded = true;

	isFacingRight = true;
	prevFacing = isFacingRight;

	jumpCount = 0;
	SetJump(jumpHeight, jumpRiseTime);
	curMoveState = MoveState::IDLE;
}

void Character::Update()
{
	prevBottom = ((RectCollider*)collider)->B();

	UpdateControl();

	animator->Update();
	UpdateCollision();

	PlayAnim();

	PostUpdate();
}

void Character::UpdateControl()
{
	Move();
}

void Character::UpdateCollision()
{
	if (jumpIgnoreGroundTime > 0.0f)
		jumpIgnoreGroundTime -= DELTA;

	if (isFalling)
	{
		dropTimer -= DELTA;
		if (dropTimer <= 0.0f)
		{
			isFalling = false;
			dropThrough = nullptr;
		}
	}

	if (stage == nullptr) return;

	UpdateWorld();
	collider->UpdateWorld();

	//floor(공중발판), object(바닥, 벽)
	FloorTileCollision();
	StairTileCollision();
	WallTileCollision();

	UpdateWorld();
	collider->UpdateWorld();
	groundSensor->UpdateWorld();

	SenseGrounded();
}

void Character::Render()
{
	SetRender();
	animator->Render();
	collider->Render();
	groundSensor->Render();
}

void Character::PostRender()
{
	RenderUI();
	collider->RenderUI();
	groundSensor->RenderUI();
}

void Character::PlayClip(const string& name)
{
	animator->PlayClip(name);
}

Clip* Character::FindClip(const string& name)
{
	return animator->FindClip(name);
}

void Character::Move()
{
	if (dir.Length() >= FLT_EPSILON)
	{
		dir.Normalize();

		velocity.x = dir.x * stat->GetStat(Attribute::MovementSpeed);
	}
	else
	{
		velocity.x = 0.0f;
	}

	if (isGrounded)
	{
		velocity.y = 0.0f;
	}
	else
	{
		velocity.y -= gravity * DELTA;

		// 낙하 속도 상한 - 프레임당 이동량이 얇은 발판 두께를 넘지 않도록
		if (velocity.y < -maxFallSpeed)
			velocity.y = -maxFallSpeed;
	}

	CheckFacing();
	Pos() += velocity * DELTA;

	ResetMove();
}

void Character::Jump()
{
	if (jumpCount >= maxJumpCount) return;

	jumpCount++;
	velocity.y = sqrt(2.0f * gravity * jumpHeight);	// 점프 순간의 현재 중력으로 초속 역산 (v = √2gh)
	isGrounded = false;
	groundFloor = nullptr;
	jumpIgnoreGroundTime = GROUND_IGNORE_TIME;
}

void Character::SetJump(float height, float riseTime)
{
	jumpHeight = height;
	jumpRiseTime = riseTime;

	// 높이 h를 시간 t 안에 도달하는 중력 역산 (g = 2h/t²)
	// 점프 초속은 저장하지 않고 Jump()에서 현재 중력으로 계산
	gravity = 2.0f * height / (riseTime * riseTime);
	maxFallSpeed = sqrt(2.0f * gravity * height) * MAX_FALL_SPEED_RATIO;
}

void Character::Fall()
{
	isGrounded = false;
	groundFloor = nullptr;
	//if (velocity.y > 0.0f) velocity.y = 0.0f;
	velocity.y = -1.0f;
	jumpIgnoreGroundTime = GROUND_IGNORE_TIME;

	if (isFalling) dropTimer = DROP_TIME;
}

void Character::KnockBack(float amount)
{
	//가로 넉백 - 거리만큼만 이동
	if (knockBackMoved < knockBackDistance)
	{
		float distance = knockBackSpeed * DELTA;
		if (knockBackMoved + distance > knockBackDistance)
			distance = knockBackDistance - knockBackMoved;

		Pos().x += knockBackDirX * distance;
		knockBackMoved += distance;
	}

	//세로(띄우기)는 velocity.y + 중력으로 Move()가 처리 → 띄워졌으면 착지할 때까지 넉백 유지
	if (knockBackMoved >= knockBackDistance && (!isLaunched || isGrounded))
	{
		isKnockBack = false;
		isLaunched = false;
		EndKnockBack();
	}
}

void Character::StartKnockBack(const HitData& hit)
{
	knockBackSpeed = knockBackDistance / knockBackDuration;
	knockBackDirX = hit.attackDir;
	knockBackMoved = 0.0f;
	isKnockBack = true;

	isLaunched = hit.launchHeight > 0.0f;
	if (isLaunched)
	{
		velocity.y = sqrt(2.0f * gravity * hit.launchHeight);	// 맞는 쪽 중력 기준으로 정확히 그 높이까지 뜸 (v = √2gh)
		isGrounded = false;
		groundFloor = nullptr;
		jumpIgnoreGroundTime = GROUND_IGNORE_TIME;	// 바로 착지 판정되는 것 방지
	}
}

void Character::EndKnockBack()
{
}

void Character::Die()
{
	SetActive(false);
}

void Character::FloorTileCollision()
{
	RectCollider* bodyCollider = (RectCollider*)collider;

	for (int iter = 0; iter < 4; ++iter)
	{
		Vector2 overlap;
		Tile* hit = stage->CheckFloorTileCollision(bodyCollider, &overlap);

		if (hit == nullptr) return;

		RectCollider* hitCol = hit->GetCollider();
		float bodyCenterX = (bodyCollider->L() + bodyCollider->R()) * 0.5f;
		float bodyCenterY = (bodyCollider->B() + bodyCollider->T()) * 0.5f;
		float TileCenterX = (hitCol->L() + hitCol->R()) * 0.5f;
		float TileCenterY = (hitCol->B() + hitCol->T()) * 0.5f;

		//겹침정도로 벽인지 바닥,천장인지 판정
		//벽
		if (overlap.x < overlap.y)
		{
			//겹침만큼 밀어내고 멈춤
			Pos().x += (bodyCenterX < TileCenterX) ? -(overlap.x + WALL_PUSH_OFFSET) : (overlap.x + WALL_PUSH_OFFSET);
			//velocity.x = 0.0f;
		}
		else
		{
			//바닥
			if (bodyCenterY > TileCenterY && prevBottom >= hitCol->T() - LANDING_ALLOW_OFFSET)
			{
				Pos().y += overlap.y;
				if (velocity.y < 0.0f)
					velocity.y = 0.0f;
			}
			////천장
			//else if (bodyCenterY < TileCenterY && velocity.y > 0.0f)
			//{
			//	Pos().y -= overlap.y;
			//	velocity.y *= -0.5f; //아래로 튕김
			//}
			else
			{
				Pos().x += (bodyCenterX < TileCenterX) ? -overlap.x : overlap.x;
			}
		}

		UpdateWorld();
		bodyCollider->UpdateWorld();
	}
}

void Character::WallTileCollision()
{
	RectCollider* bodyCollider = (RectCollider*)collider;

	for (int iter = 0; iter < 4; ++iter)
	{
		Vector2 overlap;
		Tile* hit = stage->CheckWallTileCollision(bodyCollider, &overlap);

		if (hit == nullptr) return;

		RectCollider* hitCol = hit->GetCollider();
		float bodyCenterX = (bodyCollider->L() + bodyCollider->R()) * 0.5f;
		float bodyCenterY = (bodyCollider->B() + bodyCollider->T()) * 0.5f;
		float TileCenterX = (hitCol->L() + hitCol->R()) * 0.5f;
		float TileCenterY = (hitCol->B() + hitCol->T()) * 0.5f;

		//겹침정도로 벽인지 바닥,천장인지 판정
		//벽
		if (overlap.x < overlap.y)
		{
			//겹침만큼 밀어내고 멈춤
			Pos().x += (bodyCenterX < TileCenterX) ? -(overlap.x + WALL_PUSH_OFFSET) : (overlap.x + WALL_PUSH_OFFSET);
			//velocity.x = 0.0f;
		}
		else
		{
			////바닥
			//if (bodyCenterY > TileCenterY && prevBottom >= hitCol->T() - LANDING_ALLOW_OFFSET)
			//{
			//	Pos().y += overlap.y;
			//	if (velocity.y < 0.0f)
			//		velocity.y = 0.0f;
			//}
			//천장
			if (bodyCenterY < TileCenterY && velocity.y > 0.0f)
			{
				Pos().y -= overlap.y;
				velocity.y = 0.0f; //천장에 막히면 수직 속도 제거 → 튕기지 않고 바로 낙하
			}
			else
			{
				Pos().x += (bodyCenterX < TileCenterX) ? -overlap.x : overlap.x;
			}
		}

		UpdateWorld();
		bodyCollider->UpdateWorld();
	}
}

void Character::StairTileCollision()
{
	if (velocity.y > 0.0f) return;

	RectCollider* bodyCollider = (RectCollider*)collider;

	//floor은 한방향만 검사하면된다.
	Vector2 overlap;
	Tile* hit = stage->CheckStairTileCollision(bodyCollider, &overlap);

	if (hit == nullptr) return;

	RectCollider* hitCol = hit->GetCollider();
	float tileTop = hitCol->T();

	//통과중인 타일이거나 같은 줄이면 상호작용 X
	// 떨어지는 와중에 다른 상호작용으로 올라가도 DropTimer가 dropthrough를 nullptr로 바꿈
	if (IsPassingTile(tileTop)) return;

	//이전 프레임에 착지 허용선보다 위에 있었고, 현재는 충돌했을때
	if (prevBottom >= tileTop - LANDING_ALLOW_OFFSET && bodyCollider->B() < tileTop)
	{
		Pos().y += tileTop - bodyCollider->B();
		if (velocity.y < 0.0f)
			velocity.y = 0.0f;

		UpdateWorld();
		bodyCollider->UpdateWorld();
	}
}


MoveState Character::SenseMove()
{
	if (isGrounded && velocity.x != 0) return MoveState::WALK;
	if (!isGrounded && velocity.y > 0) return MoveState::JUMP;
	if (!isGrounded && velocity.y <= 0) return MoveState::FALL;
	return MoveState::IDLE;
}

//groundSensor와 Tile의 충돌
void Character::SenseGrounded()
{
	// 점프/아래점프 직후 또는 상승 중 → 무조건 not grounded
	if (jumpIgnoreGroundTime > 0.0f || velocity.y > FLT_EPSILON)
	{
		isGrounded = false;
		groundFloor = nullptr;
		return;
	}

	//하강중

	//obj타일위 -> 그냥 착지
	if (stage->CheckFloorTileCollision(groundSensor) != nullptr)
	{
		SetGrounded(nullptr);
		return;
	}

	//floor tile 위
	Tile* floorTile = stage->CheckStairTileCollision(groundSensor);
	if (floorTile != nullptr)
	{
		float topPosY = floorTile->GetCollider()->T();

		if (!IsPassingTile(topPosY) && prevBottom >= topPosY - LANDING_ALLOW_OFFSET)
		{
			SetGrounded(floorTile);
			return;
		}
	}

	//충돌하지 않음
	isGrounded = false;
	groundFloor = nullptr;
}

void Character::SetGrounded(Tile* floor)
{
	isGrounded = true;
	jumpCount = 0;
	groundFloor = floor;
	isFalling = false;
	dropThrough = nullptr;
}

void Character::PlayAnim()
{
	if (isPlayingAction) return;

	MoveState moveState = SenseMove();

	if (moveState != curMoveState)
	{
		switch (moveState)
		{
		case MoveState::IDLE:
			PlayClip("Idle");
			break;

		case MoveState::WALK:
			PlayClip("Walk");
			break;

		case MoveState::JUMP:
			PlayClip("Jump");
			break;

		case MoveState::FALL:
			PlayClip("Fall");
			break;
		}

		curMoveState = moveState;
	}
}

void Character::CheckFacing()
{
	if (dir.x > 0) isFacingRight = true;
	else if (dir.x < 0) isFacingRight = false;

	if (prevFacing != isFacingRight)
	{
		Scale().x *= -1.0f;
		prevFacing = isFacingRight;
	}
}

void Character::OnHit(const HitData& hit)
{
	if (hit.damage > 0.0f)
	{
		bool isCritical = Random(0.0f, 1.0f) < hit.criticalChance;
		float damage = hit.damage * (isCritical ? hit.criticalDamage : 1.0f);

		//DamageTaken류는 NonBaseStat이라 기본값이 1.0(그대로 받음) - 저항/방어면 1보다 작아지고, 취약이면 1보다 커짐
		float multiplier = 1.0f;

		multiplier *= stat->GetStat(Attribute::DamageTaken);

		switch (hit.damageType)
		{
		case DamageType::Physical: multiplier *= stat->GetStat(Attribute::PhysicalDamageTaken); break;
		case DamageType::Magic:    multiplier *= stat->GetStat(Attribute::MagicDamageTaken); break;
		case DamageType::Fixed:    multiplier *= 1.0f; break;   // 방어/저항 무시
		}

		int finalDamage = (int)ceil(damage * multiplier);
		TakeDamage(finalDamage);

		//+ isCritical로 크리 이펙트/UI 표시는 여기서 처리
	}

	//+ 히트 이펙트, 경직 등은 여기서 처리
	StartKnockBack(hit);
}

void Character::TakeDamage(int amount)
{
	if (curHP <= amount)
	{
		curHP = 0;
		NotifyHPChanged();	//UI가 0을 반영한 뒤 사망 처리

		//Die
		Die();
	}
	else
	{
		curHP -= amount;
		NotifyHPChanged();
	}
}

void Character::SetMaxHP()
{
	maxHP = (int)ceil(stat->GetStat(Attribute::Health));
	NotifyHPChanged();
}

void Character::SetFacing(bool right)
{
	isFacingRight = right;
	if (prevFacing != isFacingRight)
	{
		Scale().x *= -1.0f;
		prevFacing = isFacingRight;
	}
}

void Character::OnStartAction()
{
	isPlayingAction = true;
	canCancelAction = false;
	curMoveState = MoveState::NONE;
}

void Character::OnEndAction()
{
	isPlayingAction = false;
	canCancelAction = false;
}

void Character::MoveVector2(float x, float y)
{
	dir += {x, y};
}

