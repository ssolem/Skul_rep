#include "Framework.h"
#include "RenderTargetScene.h"
#include "Objects/TestObject/TestQuad.h"

RenderTargetScene::RenderTargetScene()
{
	background = new Quad(L"Textures/sun.png");
	background->SetTag("Background");
	background->Pos() = { CENTER_X, CENTER_Y };
	background->UpdateWorld();

	airplane = new TestQuad();
	airplane->SetTag("Airplane");
	airplane->Pos() = { CENTER_X, CENTER_Y };

	//렌더 타겟 준비

	renderTarget = new RenderTarget();  //렌더 타겟 기능을 객체로 준비

	Texture* texture = Texture::Add(L"RenderTargetData", renderTarget->GetSRV());
	//만든 텍스쳐 + 렌더 타겟 + 셰이더 적용이 된 최종 화면이 그림으로서
	//텍스처 클래스 (및 디바이스) 안에서 관리된다.

	//위의 텍스처를 받아서 화면에 띄워줄 화면 속의 액자도 준비
	Quad* renderTexture;
	renderTexture = new Quad(Vector2(CENTER_X, CENTER_Y)); //일단 빈 쿼드
	renderTexture->Pos() = { CENTER_X * 0.5f, CENTER_Y * 0.5f }; //좌하
	renderTexture->UpdateWorld();
	renderTexture->SetTexture(texture); //위의 텍스처 적용
	renderTextures.push_back(renderTexture); //벡터에 추가

	renderTexture = new Quad(Vector2(CENTER_X, CENTER_Y)); //또 다른 데이터 생성
	renderTexture->Pos() = { CENTER_X * 1.5f, CENTER_Y * 0.5f }; //우하
	renderTexture->UpdateWorld();
	renderTexture->SetTexture(texture); //위의 텍스처 적용
	renderTexture->SetPixelShader(L"Filter.hlsl"); //필터 적용
	renderTextures.push_back(renderTexture); //벡터에 추가

	//화면을 위해 렌더 데이터를 구성할 때
	//렌더 타겟 또한 그 정보를 받아서 나름의 화면을 별도로 만든다.

	//필터에 옵션을 넣기 위한 상수 버퍼들도 만든다.
	intValueBuffer = new IntValueBuffer();
	intValueBuffer->Get()[0] = 0;		//index
	intValueBuffer->Get()[1] = 0;		//scale
	intValueBuffer->Get()[2] = 0;		//radial1
	intValueBuffer->Get()[3] = 0;		//radial2

	floatValueBuffer = new FloatValueBuffer();
	floatValueBuffer->Get()[0] = WIN_WIDTH;		//생성 당시 화면정보의 크기
	floatValueBuffer->Get()[1] = WIN_HEIGHT;	
}

RenderTargetScene::~RenderTargetScene()
{
	delete background;
	delete airplane;
	delete renderTarget;

	for (Quad* rt : renderTextures)
		delete rt;

	delete intValueBuffer;
	delete floatValueBuffer;
}

void RenderTargetScene::Update()
{
	airplane->Update();
}

void RenderTargetScene::PreRender()
{
	//화면의 구성 단계에서부터 렌더를 진행

	renderTarget->Set(); //화면의 복붙을 설정

	//그렇게 복사된 화면들을 뷰/투영 전에 곧바로 출력
	background->Render();
	airplane->Render();
}

void RenderTargetScene::Render()
{
	//본래 화면의 정보를 노출
	//background->Render();
	//airplane->Render();

	//렌더 타겟의 설정과 출력
	intValueBuffer->SetPS(1);	//필터의 옵션을 픽셀 셰이더에 적용
	floatValueBuffer->SetPS(2);	//크기 옵션

	for (Quad* renderTexture : renderTextures)
		renderTexture->Render();
}

void RenderTargetScene::PostRender()
{
	background->RenderUI();
	airplane->RenderUI();

	//ImGui 기능을 써서 렌더 타겟의 옵션을 조절할 수 있도록
	//Slider~~ : 막대로 옵션을 조절하는 기능, 변화가 생기면 true 반환
	ImGui::SliderInt("Index", //옵션 이름
		intValueBuffer->Get(), //바꾸려는 데이터의 참조, 포인터를 바로 주면 0번째
		0,						// 슬라이더 최소값
		8);						// 슬라이더 최대값
	ImGui::SliderInt("Scale", &intValueBuffer->Get()[1], 0, 100);
	ImGui::SliderInt("Radial1", &intValueBuffer->Get()[2], 0, 100);
	ImGui::SliderInt("Raidal2", &intValueBuffer->Get()[3], 0, 100);

	ImGui::SliderFloat("ImageSize X", floatValueBuffer->Get(), 0.0f, 2000.0f);
	ImGui::SliderFloat("ImageSize Y", &floatValueBuffer->Get()[1], 0.0f, 2000.0f);
}
