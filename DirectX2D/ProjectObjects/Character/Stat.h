#pragma once

struct StatLayer
{
	float base = 0.0f;
	float add = 0.0f;
	float percentRatio = 1.0f;
	float percentAdd = 0.0f;
};

//스탯들의 최종 배율 결정하는 클래스
class Stat
{
public:
	Stat();
	~Stat();

	void AddStat(const StatData& data);
	void RemoveStat(void* source);

	float CalculateFinalAttack(float baseDamage, DamageType damageType, AttackType attackType, AttackActionType actionType);

	float GetStat(Attribute attribute) { return finalValues[(int)attribute]; }   // O(1) 읽기

private:

	void CalculateFinalValue(StatLayer& sum, Attribute attribute);
	void RecalculateFinalValue(Attribute attribute);
	static StatCalculateType GetCalculateType(Attribute attribute);

private:

	vector<StatData> statDatas;
	vector<StatLayer> finalDatas = vector<StatLayer>((int)Attribute::Max);
	vector<float> finalValues = vector<float>((int)Attribute::Max);

};
