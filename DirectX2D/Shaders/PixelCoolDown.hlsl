struct PixelInput
{
    float4 pos : SV_POSITION;
    float2 uv : UV;
};

Texture2D icon : register(t0);

SamplerState samp : register(s0);

cbuffer ValueBuffer : register(b1)
{
    float fillAmount; // 0~1
}

float4 PS(PixelInput input) : SV_TARGET
{    
    float4 color = icon.Sample(samp, input.uv);

    //0.5. 0.5 중심 기준 -> 0,0 중심좌표로 이동
    float2 d = input.uv - 0.5f;
    
    // 12시 = 0, 시계방향 증가
    float angle = atan2(d.x, -d.y);
    float t = frac(angle / 6.2831853f + 1.0f); // 0 ~ 1

    if (t > fillAmount)
        color.rgb *= 0.35f; 

    return color;
}