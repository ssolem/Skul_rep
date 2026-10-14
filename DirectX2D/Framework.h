#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720

#define WIN_START_X 100
#define WIN_START_Y 100

#define CENTER_X (WIN_WIDTH * 0.5f)
#define CENTER_Y (WIN_HEIGHT * 0.5f)

#define DEVICE Device::Get()->GetDevice()
#define DC Device::Get()->GetDeviceContext()

#define KEY_DOWN(k) Keyboard::Get()->Down(k)
#define KEY_UP(k) Keyboard::Get()->Up(k)
#define KEY_PRESS(k) Keyboard::Get()->Press(k)

#define DELTA Timer::Get()->GetElapsedTime()

#define CAM Environment::Get()->GetMainCamera()

#define DIALOG ImGuiFileDialog::Instance()

#define SKUL_DEFAULT_ASSET_SIZE Vector2(2.0f, 2.0f)

#include <windows.h>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <functional>

#include <thread> //스레드 기능 집합 - 다른 기능과의 연계도 포함
#include <atomic> //아토믹 변수 - 접근 중 연산이 가능하고, 동시 접근이 불가능해지게 하는 변수

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

//DirectXTex
#include <DirectXTex.h>
#pragma comment(lib, "DirectXTex.lib")

//DirectWrite
#include <d2d1_2.h>
#include <dwrite.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

//ImGui
#include <ImGui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include <dirent/dirent.h>
#include <ImGuiFileDialog.h>
#include <ImGuiFileDialogConfig.h>

#pragma comment(lib, "ImGui.lib")

#include "Libraries/inc/fmod.hpp" //C++용
#pragma comment(lib, "Libraries/Lib/fmod_vc.lib")

using namespace std;
using namespace DirectX;

typedef XMFLOAT4 Float4;
typedef XMFLOAT3 Float3;
typedef XMFLOAT2 Float2;
typedef XMMATRIX Matrix;

typedef function<void()> Event;
typedef function<void(void*)> ParamEvent;
typedef function<void(int)> IntParamEvent;

//Framework Header

#include "Framework/Utilities/Singleton.h"
#include "Framework/Utilities/Keyboard.h"
#include "Framework/Utilities/Timer.h"
#include "Framework/Utilities/Utility.h"
#include "Framework/Utilities/Observer.h"
#include "Framework/Utilities/tinyxml2.h"
#include "Framework/Utilities/Font.h"
#include "Framework/Utilities/BinaryReader.h"
#include "Framework/Utilities/BinaryWriter.h"
#include "Framework/Utilities/Audio.h"

using namespace Utility;

#include "Framework/Math/Vector2.h"
#include "Framework/Math/GameMath.h"
#include "Framework/Math/Transform.h"

using namespace GameMath;

#include "Framework/System/Device.h"

#include "Framework/Shader/Shader.h"
#include "Framework/Shader/VertexShader.h"
#include "Framework/Shader/PixelShader.h"

#include "Framework/Buffer/VertexBuffer.h"
#include "Framework/Buffer/IndexBuffer.h"
#include "Framework/Buffer/ConstBuffer.h"
#include "Framework/Buffer/VertexLayouts.h"
#include "Framework/Buffer/GlobalBuffer.h"

#include "Framework/Render/Texture.h"
#include "Framework/Render/Frame.h"
#include "Framework/Render/Clip.h"
#include "Framework/Render/RenderTarget.h"

#include "Framework/Collision/Collider.h"
#include "Framework/Collision/RectCollider.h"
#include "Framework/Collision/CircleCollider.h"
#include "Framework/Collision/LineCollider.h"

#include "Framework/Environment/Camera.h"
#include "Framework/Environment/Environment.h"

#include "ProjectObjects/ProjectEnums.h"

//Object Header
#include "Objects/Basic/GameObject.h"
#include "Objects/Basic/Quad.h"
#include "Objects/Basic/Action.h"
#include "Objects/Basic/Effect.h"

#include "Objects/UI/Button.h"
#include "Objects/UI/ButtonObject.h"
#include "Objects/UI/Panel.h"
#include "Objects/UI/ProgressBar.h"
#include "Objects/UI/ImageFont.h"

#include "Objects/Navigation/DNode.h"
#include "Objects/Navigation/Dijkstra.h"
#include "Objects/Navigation/Node.h"
#include "Objects/Navigation/Heap.h"
#include "Objects/Navigation/AStar.h"

#include "Objects/Tile/Tile.h"
#include "Objects/Tile/EditTileMap.h"

#include "Objects/Manager/BulletManager.h" // 전역 총알
#include "Objects/Manager/ProjectileManager.h" // 전역 총알
#include "Objects/Manager/CommandManager.h" // 전역 커맨드 관리자
#include "Objects/Manager/EffectManager.h"
#include "Objects/Manager/PanelManager.h"
#include "Objects/Manager/DamageTextManager.h"

//Controller
#include "GameController/GameController.h"

//Scene Header
#include "Scenes/Scene.h"
#include "GameManager/GameManager.h"
#include "GameManager/SceneManager.h"

#include "ProjectObjects/Objects/SkulObject.h"

extern HWND hWnd;
extern Vector2 mousePos;
extern float mouseWheel;