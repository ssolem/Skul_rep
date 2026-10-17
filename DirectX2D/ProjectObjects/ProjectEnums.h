#pragma once

// 프로젝트 전역에서 공유하는 enum 모음
// 클래스 간 결합을 줄이려고 개별 헤더에서 이곳으로 분리

// 액션 프레임워크 식별자 (Player / Enemy 공용)
enum class ActionType
{
	BASIC_ATTACK, JUMP_ATTACK, DASH, SKILL, SWITCH, JUMP
};

// 캐릭터 이동 상태 (이동 애니메이션 전환 판정용)
enum class MoveState
{
	NONE,
	IDLE,
	WALK,
	JUMP,
	FALL
};

//SkulType
enum class SkulType
{
	BALANCE, SPEED, POWER
};

enum class DamageType
{
	Physical, Magic, Fixed
};

enum class AttackType
{
	Melee, Range, Projectile
};

enum class AttackActionType
{
	None, Basic, Skill
};

enum class DashType
{
	INVINCIBLE, SUPER_ARMOR
};

//오브젝트가 중력을 받을지, 다른 방식으로 움직일지
enum class FlightState 
{ 
	LAUNCHING, PHYSICS
};

//Stat
enum class StatType
{
	BaseStat,
	AddStat,
	PercentRatio,
	PercentAdd,
	FinalStat
};

//Attribute마다 최종값 공식이 다름 - Health/MovementSpeed처럼 실제 기준값(base)이 있는 스탯과, 순수 배율뿐인 나머지 스탯
enum class StatCalculateType
{
	HasBaseStat,   // base * percentRatio * (1+percentAdd) + add
	NonBaseStat,   // percentRatio * (1+percentAdd)
};

//스컬은 스탯이 배율형식
enum class Attribute
{
	Health = 0,
	AttackDamage, PhysicalAttackDamage, MagicAttackDamage, 
	BasicAttackDamage, SkillAttackDamage, ProjectileAttackDamage,
	DamageTaken, PhysicalDamageTaken, MagicDamageTaken, HealTaken,
	AttackSpeed, BasicAttackSpeed, SkillAttackSpeed,
	MovementSpeed, 
	ChargeAttackSpeed,
	CooldownSpeed, SkillCoolDownSpeed, SwitchCoolDownSpeed, EssenceCoolDownSpeed, DashCoolDownSpeed,
	CriticalChance, CriticalDamage,
	Max,
};

struct StatData
{
	StatType type;

	Attribute attribute;
	float value;
	void* source = nullptr;   // 이 모디파이어를 넣은 버프/장비 등 - RemoveStat에서 정확히 이 항목만 지우는 데 사용
};

//기본 공격 콤보 한 스텝(A/B/C...)에 필요한 데이터를 한 곳에 모음 (클립 이름/이동값이 인덱스로 따로 어긋나는 것 방지)
struct AttackData
{
	string clipName;
	float moveDistance = 0.0f;
	float moveDuration = 0.0f;
	int damage;

	AttackType type;
	Vector2 hitboxSize;
	Vector2 hitboxOffset = Vector2::Zero;
};

//damage가 0이면 무데미지 - OnHit()이 이 값으로 판단
//크리티컬은 맞는 대상마다 확인, 충돌판정시 hitdata구성 계산 줄이기 위해서
struct HitData
{
	float damage = 0.0f;
	float criticalChance = 0.0f;
	float criticalDamage = 1.0f;
	DamageType damageType = DamageType::Physical;
	float attackDir = 0.0f;
	void* source = nullptr;
	float launchHeight = 0.0f; 
};

enum class TileHitType
{
	Destroy,        
	Stick,          
	Pass,           
};

enum class ProjectileTarget { Player, Enemy };

//이펙트 프레임 기준점 - Bottom이면 Play 위치가 이미지 아래 가운데 (발밑 먼지, 착지 연기 등)

enum class EffectPivot { Center, Bottom };

struct ProjectileData
{
	string tag;
	wstring path;	// 아틀라스 폴더 - 투사체가 Init()에서 자기 animator에 클립 생성
	string flyClip = "Projectile";		//path 안의 비행 클립 파일 이름 (확장자 제외) - 한 폴더에 여러 투사체가 있을 때 구분
	string hitClip = "Projectile_Hit";	//소멸 클립 - 비우면 소멸 연출 없이 바로 반환
	float speed;
	float lifeTime;
	TileHitType tileHitType = TileHitType::Destroy;
	bool pierce = false;
	Vector2 size;
	float gravity = 0.0f;			//0이면 직선 비행, 값이 있으면 포물선
	bool rotateToVelocity = true;	//비행 방향으로 회전 (false면 발사 순간 각도 유지)
};


