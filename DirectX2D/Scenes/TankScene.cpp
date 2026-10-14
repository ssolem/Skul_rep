#include "Framework.h"
#include "TankScene.h"
#include "Objects/Tile/GameTileMap.h"
#include "Objects/Character/Tank/Tank.h"
#include "Objects/TestObject/TestQuad.h"


TankScene::TankScene()
{
	// 배경 및 경로 준비
	map = new GameTileMap("TextData/map1.map", { 80, 80 });

	aStar = new AStar();
	aStar->ImportNodes(map);

	tank = new Tank(aStar); // 길 찾기 능력을 넘겨주고 생성
	//tank->Pos() = { 80, 80 };
	//tank->Scale() = Vector2(1, 1) * 0.5f;

	// 탱크의 표적
	quad = new TestQuad();
	//quad->Pos() = { CENTER_X, CENTER_Y };
	//quad->Scale() = Vector2(1, 1) * 0.2f;

	//BulletManager::Get();

	// 0731 : 트랜스폼 정보 찾아서 직접 변환
	//Transform* target = static_cast<Transform*>(quad);

	//// 탱크의 표적 확인용 이벤트를 실행 -> 표적이 누군지 등록
	//// 0731 : 받아낸 트랜스폼 정보를 전달
	//Observer::Get()->ExecuteParamEvent("TankPatrolSetTarget", target);
	//Observer::Get()->ExecuteParamEvent("TankTraceSetTarget", target);
	////Observer::Get()->ExcuteParamEvent("TankAttackSetTarget", target);

	//// 0731 : 탱크 상태를 게임에서 지정
	//Observer::Get()->ExecuteIntParamEvent("TankState", (int)Tank::State::PATROL);

	//패널을 매니저로 등록해서 써보기
	//PanelManager::Get()->AddPanel("Sun", L"Textures/sun.png");
	//PanelManager::Get()->AddPanel("Sun", L"Textures/sun.png", false);
	//PanelManager::Get()->AddPanel("Baseball", L"Textures/PinBall/Ground.png");
	//PanelManager::Get()->AddPanel("Ground", L"Textures/Shooting/bgTest.png");

	imageFont = new ImageFont(L"Textures/UI/number.png");
	imageFont->Pos() = { WIN_WIDTH * 3 / 4, WIN_HEIGHT * 3 / 4 };
	imageFont->SetAlignedType(ImageFont::AlignedType::CENTER);

	//소리
	Audio::Get()->Add("BGM", "Sounds/drumloop.wav", true); //FMOD 자체는 wav아니어도 재생 가능
	//Audio::Get()->Play("BGM", 1.0f);

	//CAM->SetTarget(tank);
}

TankScene::~TankScene()
{
	//PanelManager::Get()->Delete();
	//BulletManager::Get()->Delete();

	delete imageFont;

	delete quad;
	delete tank;

	delete aStar;
	delete map;
}

void TankScene::Update()
{
	if (KEY_DOWN('1'))
		PanelManager::Get()->Show("");
	if (KEY_DOWN('2'))
		PanelManager::Get()->Show("Sun");
	if (KEY_DOWN('3'))
		PanelManager::Get()->Show("Baseball");
	if (KEY_DOWN('4'))
		PanelManager::Get()->Show("Ground");

	if (KEY_DOWN(VK_F2))
	{
		SceneManager::Get()->Add("AStar Scene");
		SceneManager::Get()->Remove("Tank Scene");
	}

	aStar->Update();
	tank->Update();
	quad->Update();

	map->PushObject(quad->GetCollider());

	BulletManager::Get()->Update();
	PanelManager::Get()->Update();

	//이미지폰트용 총알계산
	{
		UINT ammo = quad->GetAmmo();
		imageFont->SetValue(ammo);
		imageFont->Update();
	}
}

void TankScene::Render()
{
	map->Render();
	aStar->Render(); // 옵션

	tank->Render();
	quad->Render();

	BulletManager::Get()->Render();
}

void TankScene::PostRender()
{
	tank->PostRender();
	quad->PostRender();
	quad->RenderUI();

	PanelManager::Get()->Render();

	imageFont->Render();
}

void TankScene::Start()
{
	//생성자와 별개로 씬 세부 세팅
	tank->Pos() = { 80, 80 };
	tank->Scale() = Vector2(1, 1) * 0.5f;

	quad->Pos() = { CENTER_X, CENTER_Y };
	quad->Scale() = Vector2(1, 1) * 0.2f;
	quad->RefillAmmo();

	//탱크에게 표적 주는 후속 설정
	Transform* target = static_cast<Transform*>(quad);
	Observer::Get()->ExecuteParamEvent("TankPatrolSetTarget", target);
	Observer::Get()->ExecuteParamEvent("TankTraceSetTarget", target);
	Observer::Get()->ExecuteIntParamEvent("TankState", (int)Tank::State::PATROL);
	
	//브금 재생
	Audio::Get()->Play("BGM", 1.0f);

	//UI 생성
	PanelManager::Get()->AddPanel("Sun", L"Textures/sun.png", false);
	PanelManager::Get()->AddPanel("Baseball", L"Textures/PinBall/Ground.png");
	PanelManager::Get()->AddPanel("Ground", L"Textures/Shooting/bgTest.png");
}

void TankScene::End()
{
	//소멸자에서 전담하던 데이터 해제 일부
	//Start 에서 만든 객체, 씬이 종료되면 삭제되지 않아도 내부에서 지워져야하는 객체

	//Start에서 생성했다면
	//delete tank;
	//delete quad;

	PanelManager::Get()->Delete();
	BulletManager::Get()->Delete();
}

void TankScene::Reset()
{
	End();

	//씬 사이의 간격에서만 실행되어야 하는 특수상황이 있다면 여기서

	Start();
}
