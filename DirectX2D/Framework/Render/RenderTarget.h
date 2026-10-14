#pragma once

//렌더 타겟 : 지정된 대상을 재해석 후 화면에 도출, 병합
class RenderTarget
{
public:
	RenderTarget(UINT width = WIN_WIDTH, UINT height = WIN_HEIGHT);
	~RenderTarget();

	void Set(Float4 clearColor = Float4(1, 1, 1, 1));

	ID3D11ShaderResourceView* GetSRV() { return srv; }

private:

	//화면 정보를 (재)생성하기 위한 함수들
	void CreateRTVTexture();
	void CreateRTV();
	void CreateSRV();

private:

	UINT width, height;

	//렌더 타겟에 필요한 것 : 화면의 현재 모습(그래픽 정보)
	//						렌더 타겟에 사용되는, 전용 시야 데이터
	//						렌더 타겟이 적용된 화면(텍스쳐)

	ID3D11ShaderResourceView* srv;
	ID3D11RenderTargetView* rtv;
	ID3D11Texture2D* rtvTexture;
};

