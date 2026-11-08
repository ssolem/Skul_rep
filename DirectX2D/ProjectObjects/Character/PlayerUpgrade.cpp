#include "Framework.h"
#include "PlayerUpgrade.h"
#include "Player.h"
#include "Stat.h"

const UpgradeInfo upgradeInfo[(int)UpgradeType::Max] =
{
	//치명타 확률만 다크쿼츠, 나머지는 골드
	{ CurrencyType::Gold,       Attribute::Health, StatType::AddStat, 20.0f, 100, 10, 5 },
	{ CurrencyType::Gold,       Attribute::PhysicalAttackDamage, StatType::PercentAdd, 0.02f, 100, 10, 5 },
	{ CurrencyType::Gold,       Attribute::MagicAttackDamage, StatType::PercentAdd, 0.02f, 100, 10, 5 },
	{ CurrencyType::DarkQuartz, Attribute::CriticalChance, StatType::AddStat, 0.01f, 100, 10, 5 },
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

	return GetOwnedCurrency(GetInfo(type).currency) >= GetCost(type);
}

bool PlayerUpgrade::TryUpgrade(UpgradeType type)
{
	if (!CanUpgrade(type)) return false;

	const UpgradeInfo& info = GetInfo(type);

	SpendCurrency(info.currency, GetCost(type));
	levels[(int)type]++;

	owner->GetStatComponent()->AddStat({ info.statType, info.attribute, info.valuePerLevel, this });

	if (type == UpgradeType::Health)
		RefreshHP();

	return true;
}

int PlayerUpgrade::GetOwnedCurrency(CurrencyType currency)
{
	switch (currency)
	{
	case CurrencyType::Gold:		return owner->GetGold();
	case CurrencyType::DarkQuartz:	return owner->GetDarkQuartz();
	}
	return 0;
}

void PlayerUpgrade::SpendCurrency(CurrencyType currency, int amount)
{
	switch (currency)
	{
	case CurrencyType::Gold:		owner->AddGold(-amount); break;
	case CurrencyType::DarkQuartz:	owner->AddDarkQuartz(-amount); break;
	}
}

void PlayerUpgrade::RefreshHP()
{
	int prevMaxHP = owner->GetMaxHP();
	owner->SetMaxHP();

	int diff = owner->GetMaxHP() - prevMaxHP;
	if (diff > 0)
		owner->SetCurHP(min(owner->GetCurHP() + diff, owner->GetMaxHP()));
}
