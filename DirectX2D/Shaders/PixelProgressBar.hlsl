struct PixelInput
{
    float4 pos : SV_POSITION;
    float2 uv : UV;
};

//Texture2D : GLSL, DX 와 명명을 공유하는 그림 이미지 데이터
Texture2D frontMap : register(t0);
Texture2D backMap : register(t1);

//SamplerState : 픽셀 표시에 따른 설정 데이터
SamplerState samp : register(s0);

//추가로 C++에서 상수하나를 추가로 받아 셰이더 연산에 사용
cbuffer ValueBuffer : register(b1)
{
    float fillAmount;
}

//진입점 속성에서 이름 바꾼 후 작성함
float4 PS(PixelInput input) : SV_TARGET
{
    if (input.uv.x <= fillAmount) //받은 위치가 '잔량' 수치를 못 넘으면
        return frontMap.Sample(samp, input.uv); //앞쪽 그림을 픽셀에 표시
    
    return backMap.Sample(samp, input.uv); //잔량 수치를 넘으면 뒷쪽 그림을 픽셀에 표시
}