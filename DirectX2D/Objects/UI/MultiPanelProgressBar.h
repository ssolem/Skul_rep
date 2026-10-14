#pragma once

class MultiPanelProgressBar : public GameObject
{
public:
	enum class ChangeMode
	{
		INSTANT_CHANGE,   //진행 중인 연출을 즉시 끝내고 새 연출 시작
		DELAYED_CHANGE,   //진행 중인 연출에 이어서 진행
	};

	//층별 텍스처 경로 - base/health는 필수, 나머지는 비워두면 그 층을 표시하지 않음
	struct Textures
	{
		wstring base;
		wstring health;
		wstring heal;
		wstring shield;
		wstring damage;
	};

	MultiPanelProgressBar(const Textures& textures, Vector2 barSize);
	~MultiPanelProgressBar();

	void Update();
	void Render();

	void SetBarSize(Vector2 size);	//게이지가 채울 크기 (텍스처 픽셀 단위, 부모 Scale 적용 전)
	Vector2 GetBarSize() { return barSize; }

	void Init(float maxHP, float curHP);	//연출 없이 즉시 맞춤
	void SetHP(float hp);					//줄면 데미지 연출, 늘면 힐 연출
	void SetMaxHP(float maxHP);
	void SetShield(float shield);			//연출 없음
	void SetChangeMode(ChangeMode mode) { changeMode = mode; }

	void SetFollowDelay(float delay) { followDelay = delay; }
	void SetFollowSpeed(float speed) { followSpeed = speed; }

private:
	void UpdateDamageTrail();
	void UpdateHealFill();
	void UpdateAmountBuffer();

	Texture* LoadOptional(const wstring& file);
	void BindLayer(Texture* layer, UINT slot);

private:
	Quad* gauge;		//base 텍스처를 가진 Quad - 셰이더가 나머지 층을 구간별로 고름
	Vector2 barSize = Vector2::One;

	Texture* healthImage = nullptr;
	Texture* healImage = nullptr;
	Texture* shieldImage = nullptr;
	Texture* damageImage = nullptr;

	FloatValueBuffer* amountBuffer;		//[체력색 끝, 실제 체력 끝, 실드 끝, 잔상 끝] (0~1)

	ChangeMode changeMode = ChangeMode::INSTANT_CHANGE;

	//항상 shownHP <= curHP <= trailHP

	float maxHP = 1.0f;
	float curHP = 1.0f;		//실제 체력
	float shownHP = 1.0f;	//체력색 끝 - 힐 연출 중엔 curHP보다 작음
	float trailHP = 1.0f;	//데미지 잔상 끝 - 피격 연출 중엔 curHP보다 큼
	float shield = 0.0f;

	float damageDelayTimer = 0.0f;
	float healDelayTimer = 0.0f;

	float followDelay = 0.3f;	//연출이 움직이기 시작할 때까지
	float followSpeed = 1.5f;	//연출 속도 (최대 체력 대비 초당 비율)
};
