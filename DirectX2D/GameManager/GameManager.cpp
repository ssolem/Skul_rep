#include "Framework.h"

#include "ScenesSetting.h"
#include "d3d11sdklayers.h"
#include "ProjectObjects/Character/Animator.h"

GameManager::GameManager()
{
    Create();
    uiViewBuffer = new MatrixBuffer();

	//scene = new ButtonScene();
	//scene = new ButtonPuzzleScene();
    //scene = new NinjaScene();
    //scene = new LineScene();
    //scene = new CollisionScene();
    //scene = new RenderTargetScene();
    //scene = new TileMapEditScene();
    //scene = new DijkstraScene();
    //scene = new AStarScene();
    //scene = new TestMapScene();
    //scene = new TankScene();
    //scene = new TestCharacterScene();

    //씬 매니저로 씬 생성
    //SceneManager::Get()->Create("AStar Scene", new AStarScene());
    //SceneManager::Get()->Create("Tank Scene", new TankScene());
    SceneManager::Get()->Create("TestCharacter Scene", new TestCharacterScene());
    SceneManager::Get()->Create("Atlas Scene", new AtlasMakerScene());
	SceneManager::Get()->Create("SkulMapEdit Scene", new SkulMapEditScene());
	SceneManager::Get()->Create("SpawnEdit Scene", new SpawnEditScene());

    //원하는 씬을 골라 시작
    SceneManager::Get()->Add("TestCharacter Scene"); // 탱크 씬 예약 -> 실행 -> 시작


}

GameManager::~GameManager()
{
    //delete scene;

    delete uiViewBuffer;
    Delete();
}

void GameManager::Update()
{
    if (KEY_DOWN('P'))
        Collider::isRender = !Collider::isRender;

    Keyboard::Get()->Update();
    Timer::Get()->Update();

    //scene->Update();
    SceneManager::Get()->Update();

    CAM->Update();

    mouseWheel = 0.0f;
}

void GameManager::Render()
{
    //scene->PreRender();
    SceneManager::Get()->PreRender();
    
    Device::Get()->Clear();
    Font::Get()->GetDC()->BeginDraw();

    Environment::Get()->SetViewport();
    Environment::Get()->SetProjection();

    //scene->Render();
    SceneManager::Get()->Render();

    // -> 렌더 후, ImGui 프레임 생성
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    string fps = "FPS : " + to_string(Timer::Get()->GetFPS());
    Font::Get()->RenderText(fps, { 100, WIN_HEIGHT - 10 });

    CAM->RenderUI();

    uiViewBuffer->SetVS(1);
    //scene->PostRender();
    SceneManager::Get()->PostRender();

    // -> 생성된 프레임에서, scene의 PostRender()까지 받아 ImGui 출력
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    Font::Get()->GetDC()->EndDraw();

    Device::Get()->Present();
}

void GameManager::Create()
{
    Keyboard::Get();
    Timer::Get();
    Device::Get();
    Environment::Get();
    Observer::Get();

    Font::Get()->AddColor("White", 1, 1, 1);
    Font::Get()->AddColor("Orange", 1, 0.55f, 0.1f);
    Font::Get()->AddColor("Red", 1, 0.3f, 0.3f);
    Font::Get()->AddColor("Blue", 0.2f, 0.6f, 1);

    Font::Get()->AddStyle("Default", L"맑은 고딕");
    Font::Get()->AddStyle("Damage", L"맑은 고딕", 28, DWRITE_FONT_WEIGHT_BOLD);
    Font::Get()->AddStyle("DamageCritical", L"맑은 고딕", 36, DWRITE_FONT_WEIGHT_HEAVY);

    Font::Get()->SetColor("White");
    Font::Get()->SetStyle("Default");

    // Init. ImGui
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(DEVICE, DC);
}

void GameManager::Delete()
{
    //성능상 열위, 위계상 안전
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    SceneManager::Delete();
    EffectManager::Delete();
    Animator::ClearFrameCache();   // 공유 프레임(GPU 버퍼) 정리 - Device 삭제 전에
    Keyboard::Delete();
    Timer::Delete();
    Device::Delete();
    Shader::Delete();
    Texture::Delete();
    Environment::Delete();    
    Observer::Delete();
    Font::Delete();

    //빠른편, 자원 안정성 조금 떨어짐
    //ImGui_ImplDX11_Shutdown();
    //ImGui_ImplWin32_Shutdown();
    //ImGui::DestroyContext();
}
