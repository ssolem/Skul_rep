#pragma once

class Player;

enum class UpgradeType
{
	Health,
	PhysicalAttack,
	MagicAttack,
	CriticalChance,
	Max,
};

enum class CurrencyType
{
	Gold,
	DarkQuartz,
};

struct UpgradeInfo
{
	CurrencyType currency;
	Attribute attribute;
	StatType statType;
	float valuePerLevel;
	int maxLevel;
	int baseCost;
	int costPerLevel;
};

class PlayerUpgrade
{
public:
	PlayerUpgrade(Player* owner);
	~PlayerUpgrade();

	bool CanUpgrade(UpgradeType type);
	bool TryUpgrade(UpgradeType type);	

	int GetLevel(UpgradeType type) { return levels[(int)type]; }
	int GetMaxLevel(UpgradeType type) { return GetInfo(type).maxLevel; }
	bool IsMaxLevel(UpgradeType type) { return GetLevel(type) >= GetMaxLevel(type); }
	int GetCost(UpgradeType type);
	float GetBonus(UpgradeType type) { return GetInfo(type).valuePerLevel * GetLevel(type); }

	static const UpgradeInfo& GetInfo(UpgradeType type);

private:
	void RefreshHP();
	int GetOwnedCurrency(CurrencyType currency);	//플레이어가 가진 해당 재화 양
	void SpendCurrency(CurrencyType currency, int amount);

private:
	Player* owner = nullptr;
	int levels[(int)UpgradeType::Max] = {};
};
