#include "Framework.h"
#include "Stat.h"

Stat::Stat()
{
	//스탯을 하나도 안 넣은 속성도 기본값을 갖도록 전부 한 번 계산해둠 (배율형은 1.0, 기준값형은 0)
	//안 하면 finalValues가 0으로 시작해서 AttackDamage, DamageTaken 같은 배율이 0이 되어 대미지가 0이 됨
	for (int i = 0; i < (int)Attribute::Max; i++)
		CalculateFinalValue(finalDatas[i], (Attribute)i);
}

Stat::~Stat()
{
}

void Stat::AddStat(const StatData& data)
{
	statDatas.push_back(data);

	StatLayer& sum = finalDatas[(int)data.attribute];
	switch (data.type)
	{
	case StatType::BaseStat:     sum.base += data.value; break;
	case StatType::AddStat:      sum.add += data.value; break;
	case StatType::PercentRatio: sum.percentRatio *= data.value; break; //증폭
	case StatType::PercentAdd:   sum.percentAdd += data.value; break; //증가
	case StatType::FinalStat:    break;
	}

	CalculateFinalValue(sum, data.attribute);
}

void Stat::RemoveStat(void* source)
{
	vector<Attribute> affected;

	for (auto it = statDatas.begin(); it != statDatas.end(); )
	{
		if (it->source == source)
		{
			if (find(affected.begin(), affected.end(), it->attribute) == affected.end())
				affected.push_back(it->attribute);

			it = statDatas.erase(it);
		}
		else
		{
			++it;
		}
	}

	for (Attribute attribute : affected)
		RecalculateFinalValue(attribute);
}

//HitData에 넘겨줄 damage 계산, 이후 받는 쪽에서 받는 피해와 크리티컬 적용
float Stat::CalculateFinalAttack(float baseDamage, DamageType damageType, AttackType attackType, AttackActionType actionType)
{
	float final = 1.0f;

	switch (damageType)
	{
	case DamageType::Physical: final += GetStat(Attribute::PhysicalAttackDamage) - 1.0f; break;
	case DamageType::Magic:    final += GetStat(Attribute::MagicAttackDamage) - 1.0f; break;
	case DamageType::Fixed:    break;
	}

	switch (actionType)
	{
	case AttackActionType::Basic: final += GetStat(Attribute::BasicAttackDamage) - 1.0f; break;
	case AttackActionType::Skill: final += GetStat(Attribute::SkillAttackDamage) - 1.0f; break;
	case AttackActionType::None:  break;
	}

	if (attackType == AttackType::Projectile)
		final += GetStat(Attribute::ProjectileAttackDamage) - 1.0f;

	final *= GetStat(Attribute::AttackDamage);

	return final * baseDamage;
}

void Stat::CalculateFinalValue(StatLayer& sum, Attribute attribute)
{
	finalDatas[(int)attribute] = sum;

	if (GetCalculateType(attribute) == StatCalculateType::HasBaseStat)
		finalValues[(int)attribute] = sum.base * sum.percentRatio * (1.0f + sum.percentAdd) + sum.add;
	else
		finalValues[(int)attribute] = sum.percentRatio * (1.0f + sum.percentAdd);
}

void Stat::RecalculateFinalValue(Attribute attribute)
{
	StatLayer sum;

	for (const auto& data : statDatas)
	{
		if (data.attribute != attribute) continue;

		switch (data.type)
		{
		case StatType::BaseStat:     sum.base += data.value; break;
		case StatType::AddStat:      sum.add += data.value; break;
		case StatType::PercentRatio: sum.percentRatio *= data.value; break;
		case StatType::PercentAdd:   sum.percentAdd += data.value; break;
		case StatType::FinalStat:    break;
		}
	}

	CalculateFinalValue(sum, attribute);
}

StatCalculateType Stat::GetCalculateType(Attribute attribute)
{
	switch (attribute)
	{
	case Attribute::Health:
	case Attribute::MovementSpeed:
	case Attribute::CriticalChance:	//확률이라 배율형(기본 1.0)이면 100% 크리가 됨 -> 기준값 0에서 시작
		return StatCalculateType::HasBaseStat;

	default:
		return StatCalculateType::NonBaseStat;
	}
}
