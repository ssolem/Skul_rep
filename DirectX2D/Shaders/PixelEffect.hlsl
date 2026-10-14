struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 uv : UV;
};

Texture2D map : register(t0);		// 그림 데이터
SamplerState samp : register(s0);	// 샘플(합성) 데이터

cbuffer ColorBuffer : register(b0)  // 컬러 데이터
{
	float4 color;
}

cbuffer FrameBuffer : register(b1)
{
	float2 maxFrame;
	float2 curFrame;
}

float4 PS(PixelInput input) : SV_TARGET
{
	float2 uv = input.uv + (curFrame / maxFrame);
	float4 tex = map.Sample(samp, uv);				// 합성의 대상화
	
	return tex * color;								// 화소(색) 합성
}