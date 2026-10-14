#include "Framework.h"
#include "SkulObject.h"
#include "ProjectObjects/Stage.h"

SkulObject::SkulObject(wstring file) :
	Quad(file), collider(nullptr), isGrounded(false), mass(1.0f), friction(10.0f),
	velocity(Vector2::Zero), gravity(Vector2(0.0f, -GRAVITY)), force(Vector2::Zero),
	acceleration(Vector2::Zero), maxVelocity(Vector2(200.0f, 1000.0f))
{
}

SkulObject::~SkulObject()
{
	if (collider != nullptr)
		delete collider;
}

void SkulObject::Update()
{
	if (collider != nullptr)
		prevBottom = ((RectCollider*)collider)->B(); //위에서 착지했는지 판정용 (stair 통과 처리에 사용)

	if (flightState == FlightState::LAUNCHING)
	{
		if (launchMethod) launchMethod(this, DELTA);
		UpdateWorld();
		collider->UpdateWorld();
		return;
	}

	// a = f / m;
	acceleration = force / mass;
	velocity += acceleration * DELTA;

	if (isGrounded)
	{
		// 땅위에 있을때
		Vector2 gForce = gravity;
		gForce.Normalize();

		float dot = Dot(velocity, gForce);
		velocity -= gForce * dot;
	}
	else
	{
		// 공중에 있을떄
		velocity += gravity * DELTA;
	}


	//최대 속도 제한
	Vector2 gForce = gravity;
	gForce.Normalize();
	float dot = Dot(velocity, gForce);
	gForce = gForce * dot;

	Vector2 sideVelocity = velocity - gForce;
	if (maxVelocity.y < gForce.Length())
	{
		gForce.Normalize();
		gForce *= maxVelocity.y;
	}

	if (maxVelocity.x < sideVelocity.Length())
	{
		sideVelocity.Normalize();
		sideVelocity *= maxVelocity.x;
	}
	velocity = gForce + sideVelocity;


	if (!(velocity == Vector2::Zero))
	{
		//속도에 반대방향으로 마찰력 작용
		Vector2 fForce = -velocity;
		fForce.Normalize();
		fForce = fForce * friction * mass * DELTA;

		// 마찰력으로 인한 속도 감소량이 현재 속도보다 큰 경우
		if (velocity.Length() <= fForce.Length())
		{
			velocity = Vector2::Zero;
		}
		else
		{
			velocity += fForce;
		}
	}

	angularVelocity -= angularVelocity * rotationDamping * DELTA;
	visualAngle += angularVelocity * DELTA;

	Pos() += velocity * DELTA;
	UpdateWorld();

	force = Vector2::Zero;

	collider->UpdateWorld();
}

void SkulObject::Render()
{
	float savedAngle = Rot().z;
	Rot().z = visualAngle;
	UpdateWorld();

	Quad::Render();

	Rot().z = savedAngle;
	UpdateWorld();

	collider->Render();
}

void SkulObject::RenderUI()
{

}

void SkulObject::Bounce(Vector2 normal, float decreaseRate, float randomRangeDeg)
{
	float dot = Dot(velocity, normal);
	velocity -= normal * (1.0f + decreaseRate) * dot;

	if (randomRangeDeg > 0.0f)
		velocity = GetRandomDirection(velocity.Angle(), randomRangeDeg, velocity.Length());

	//충돌로 생긴 회전력 -> 질량(관성) 클수록 적게 붙음
	float impulse = Cross(normal, velocity) * mass;
	angularVelocity += impulse / momentOfInertia;
}

void SkulObject::KnockAway(SkulObject* other)
{
	//적과 부딪혀서 Bounce로 멈추는 것과 달리, 나는 그대로 두고 상대만 날림
	bool rightDir = (other->Pos().x >= Pos().x);
	float centerAngle = rightDir ? XMConvertToRadians(45.0f) : XMConvertToRadians(135.0f);

	other->SetVelocity(GetRandomDirection(centerAngle, 80.0f, velocity.Length() * 0.6f));
	other->ActivatePhysics();
}

void SkulObject::StartLaunch(LaunchMethod method)
{
	flightState = FlightState::LAUNCHING;
	launchMethod = method;
}

void SkulObject::ActivatePhysics()
{
	flightState = FlightState::PHYSICS;
}

Vector2 SkulObject::GetRandomDirection(float centerAngle, float rangeDeg, float speed)
{
	float half = XMConvertToRadians(rangeDeg * 0.5f);
	float angle = centerAngle + Random(-half, half);
	return Vector2(cos(angle), sin(angle)) * speed;
}

void SkulObject::ResolveTileCollision(Stage* stage)
{
	RectCollider* bodyCollider = (RectCollider*)collider;

	Vector2 overlap;

	if (Tile* wall = stage->CheckWallTileCollision(bodyCollider, &overlap))
	{
		ActivatePhysics();

		Vector2 normal = (overlap.x < overlap.y) ? Vector2(velocity.x > 0 ? -1.f : 1.f, 0.f)
			: Vector2(0.f, velocity.y > 0 ? -1.f : 1.f);

		//겹친 만큼 밀어내기 - 안 하면 벽에 박힌 채로 남아서 매 프레임 재충돌함
		float penetration = (overlap.x < overlap.y) ? overlap.x : overlap.y;
		Pos() += normal * penetration;
		UpdateWorld();
		bodyCollider->UpdateWorld();

		Bounce(normal, 0.8f, 40.0f);
	}

	bool grounded = false;
	const float restSpeed = 50.0f; //이보다 느리게 떨어지면 튕기지 않고 멈춘 걸로 침

	//floor: 캐릭터와 동일하게 사방으로 막힘
	if (stage->CheckFloorTileCollision(bodyCollider, &overlap))
	{
		ActivatePhysics();

		Pos() += Vector2(0.f, 1.f) * overlap.y;
		UpdateWorld();
		bodyCollider->UpdateWorld();

		if (abs(velocity.y) < restSpeed)
		{
			SetVelocity({ velocity.x, 0.0f });
			grounded = true;
		}
		else
		{
			Bounce(Vector2(0.f, 1.f), 0.6f, 160.0f); //위쪽 기준 160도 -> 1,2사분면에 80도씩 걸침
		}
	}
	//stair: Character::StairTileCollision과 동일하게, 위에서 착지할 때만 막고 옆/아래는 통과
	else if (velocity.y <= 0.0f)
	{
		if (Tile* stair = stage->CheckStairTileCollision(bodyCollider, &overlap))
		{
			RectCollider* hitCol = stair->GetCollider();
			float tileTop = hitCol->T();

			//직전 프레임에 착지 허용선보다 위에 있었을 때만 "위에서 착지"로 인정
			if (prevBottom >= tileTop - 6.0f && bodyCollider->B() < tileTop)
			{
				ActivatePhysics();

				Pos().y += tileTop - bodyCollider->B();
				UpdateWorld();
				bodyCollider->UpdateWorld();

				if (abs(velocity.y) < restSpeed)
				{
					SetVelocity({ velocity.x, 0.0f });
					grounded = true;
				}
				else
				{
					Bounce(Vector2(0.f, 1.f), 0.6f, 160.0f);
				}
			}
			//조건을 못 채우면(옆/아래에서 온 경우) 아무 보정 없이 그대로 통과
		}
	}

	SetGround(grounded); //바닥/계단 위가 아니면 다시 중력 받게

	//다른 스컬 오브젝트/적과의 상호작용은 여기서 하지 않음 - 던진 쪽(예: BasicSkul)이 직접 검사해서 처리
}
