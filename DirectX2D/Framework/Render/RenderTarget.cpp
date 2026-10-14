#include "Framework.h"

RenderTarget::RenderTarget(UINT width, UINT height)
	: width(width), height(height)
{
	CreateRTVTexture();
	CreateRTV();
	CreateSRV();
}

RenderTarget::~RenderTarget()
{
	rtvTexture->Release(); //gdi 기능과 같다.
	rtv->Release();
	// srv : 디바이스 관리
}

void RenderTarget::Set(Float4 clearColor)
{
	//"화면을 하나로 합친다" -> 화면 병합기(output merge)

	DC->OMSetRenderTargets(1, &rtv, nullptr);
	DC->ClearRenderTargetView(rtv, (float*)&clearColor);
}

void RenderTarget::CreateRTVTexture()
{
	D3D11_TEXTURE2D_DESC desc = {}; //2D 텍스쳐 정보(옵션) 준비

	desc.Width = width; //새화면 크기
	desc.Height = height;

	desc.MipLevels = 1; //밉맵 단계
	desc.ArraySize = 1; //스프라이트 개수

	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	//전체 화면을 작성
	DEVICE->CreateTexture2D(&desc, nullptr, &rtvTexture);
}

void RenderTarget::CreateRTV()
{
	D3D11_RENDER_TARGET_VIEW_DESC desc = {};

	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	//만들어진 화면에 들어갈 정보(모습)를 작성
	DEVICE->CreateRenderTargetView(rtvTexture, &desc, &rtv);
}

void RenderTarget::CreateSRV()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};

	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = 1;

	DEVICE->CreateShaderResourceView(rtvTexture, &desc, &srv);
}
