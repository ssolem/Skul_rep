struct PixelInput
{
    float4 pose : SV_Position;
    float2 uv : UV;
};

Texture2D baseMap : register(t0); 
Texture2D healthMap : register(t1);
Texture2D healMap : register(t2);
Texture2D shieldMap : register(t3);
Texture2D damageMap : register(t4);

SamplerState samp : register(s0);

cbuffer AmountBuffer : register(b1)
{
    float shownHealth;
    float realHealth;
    float shield;
    float damage;
}

float4 PS(PixelInput input) : SV_TARGET
{
    if (input.uv.x <= shownHealth)
        return healthMap.Sample(samp, input.uv);
    if (input.uv.x <= realHealth)
        return healMap.Sample(samp, input.uv);
    if (input.uv.x <= shield)
        return shieldMap.Sample(samp, input.uv);
    if (input.uv.x <= damage)
        return damageMap.Sample(samp, input.uv);
    
    return baseMap.Sample(samp, input.uv);
}