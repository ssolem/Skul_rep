#include "Framework.h"
#include "PlayerUpgrade.h"
#include "Player.h"
#include "Stat.h"

const UpgradeInfo upgradeInfo[(int)UpgradeType::Max] =
{
	{ Attribute::Health, StatType::AddStat, 20.0f, 100, 10, 5 },	
	{ Attribute::PhysicalAttackDamage, StatType::PercentAdd, 0.02f, 100, 10, 5 },	
	{ Attribute::MagicAttackDamage, StatType::PercentAdd, 0.02f, 100, 10, 5 },	
	{ Attribute::CriticalChance, StatType::AddStat, 0.01f, 100, 10, 5 },	
};

PlayerUpgrade::PlayerUpgrade(Player* owner)
	: owner(owner)
{
}

PlayerUpgrade::~PlayerUpgrade()
{
}

const UpgradeInfo& PlayerUpgrade::GetInfo(UpgradeType type)
{
	return upgradeInfo[(int)type];
}

int PlayerUpgrade::GetCost(UpgradeType type)
{
	const UpgradeInfo& info = GetInfo(type);
	return info.baseCost + info.costPerLevel * GetLevel(type);
}

bool PlayerUpgrade::CanUpgrade(UpgradeType type)
{
	if (IsMaxLevel(type)) return false;

	return owner->GetDarkQuartz() >= GetCost(type);
}

bool PlayerUpgrade::TryUpgrade(UpgradeType type)
{
	if (!CanUpgrade(type)) return false;

	owner->AddDarkQuartz(-GetCost(type));	
	levels[(int)type]++;

	const UpgradeInfo& info = GetInfo(type);
	owner->GetStatComponent()->AddStat({ info.statType, info.attribute, info.valuePerLevel, this });

	if (type == UpgradeType::Health)
		RefreshHP();

	return true;
}

void PlayerUpgrade::RefreshHP()
{
	int prevMaxHP = owner->GetMaxHP();
	owner->SetMaxHP();

	int diff = owner->GetMaxHP() - prevMaxHP;
	if (diff > 0)
		owner->SetCurHP(min(owner->GetCurHP() + diff, owner->GetMaxHP()));
}
