#pragma once

class Stage;
class Character;
class Tile;

//바닥에 떨어지는 아이템 공통
class DropItem : public GameObject
{
protected:
	enum class State
	{
		Drop,		
		Rest		
	};

public:
	DropItem();
	virtual ~DropItem();

	void Update();
	void Render();

	//Stage::SpawnItem에서 호출
	void Spawn(Stage* stage, Vector2 pos);

protected:
	void SetIdleClip(const wstring& atlasPath);
	//애니메이션 없이 아틀라스 프레임 중 하나를 Spawn마다 돌아가며 고름 (뼈/다크쿼츠)
	void SetImages(const wstring& atlasPath);
	//획득 시 아이템 위치에서 재생할 이펙트 - 처음 한 번만 등록
	void SetAcquireEffect(const string& key, const wstring& atlasPath, float speed = 3.0f);

	virtual void OnAcquire(Character* player) = 0;

private:
	void UpdateDrop();
	void Acquire();

	void ResolveTileCollision();
	void PushOut(Tile* tile, Vector2 overlap);
	void Land();

	void CreateCollider(const vector<Frame*>& frames);	//가장 큰 프레임 기준
	Frame* PickImage();

protected:
	Clip* idleClip = nullptr;			//반복 애니메이션 (골드)
	vector<Frame*>* images = nullptr;	//낱장 후보 - 프레임은 Animator 캐시 소유
	Frame* image = nullptr;				//이번 Spawn에 고른 낱장
	string acquireEffectKey;
	RectCollider* collider = nullptr;
	Stage* stage = nullptr;

	State state = State::Drop;
	Vector2 velocity = Vector2::Zero;
	int bounceCount = 0;
	float lifeTime = 0.0f;			
	float prevBottom = 0.0f;		

	//튀어나오기
	float spreadAngle = 70.0f;		
	float minPower = 100.0f;
	float maxPower = 300.0f;

	//튕김
	float bounceRate = 0.4f;		
	float groundFriction = 0.6f;	
	float wallBounceRate = 0.5f;	
	int maxBounce = 2;				

	//자동 획득
	float acquireDelay = 1.5f;
};
