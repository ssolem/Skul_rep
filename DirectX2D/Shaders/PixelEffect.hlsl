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

float4 PS(PixelInput input) : SV_TARGET
{
	//UV는 Frame 정점에 이미 프레임 영역으로 들어 있음 - 격자 계산 불필요
	float4 tex = map.Sample(samp, input.uv);
	
	return tex * color;								// 화소(색) 합성
}