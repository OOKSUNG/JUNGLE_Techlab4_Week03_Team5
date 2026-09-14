Texture2D AtlasTex : register(t0);
SamplerState AtlasSampler : register(s0);

cbuffer TextDataWorld : register(b0)
{
    matrix VP;
    float4 Color;
    float PxRange;
    float3 Pad;
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

VS_OUTPUT mainVS(VS_INPUT Input)
{
    VS_OUTPUT Output;
    Output.Position = mul(float4(Input.Position, 1.0f), VP);
    Output.TexCoord = Input.TexCoord;
    return Output;
}

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

float4 mainPS(VS_OUTPUT Input) : SV_TARGET
{
    float3 Msd = AtlasTex.Sample(AtlasSampler, Input.TexCoord).rgb;
    float Sd = median(Msd.r, Msd.g, Msd.b) - 0.5f;

    float2 AtlasDim;
    AtlasTex.GetDimensions(AtlasDim.x, AtlasDim.y);
    float2 UnitRange = float2(PxRange, PxRange) / AtlasDim;
    float2 ScreenTexSize = 1.0f / fwidth(Input.TexCoord);
    float ScreenPxRange = max(0.5f * dot(UnitRange, ScreenTexSize), 1.0f);

    float Opacity = saturate(ScreenPxRange * Sd + 0.5f);
    return float4(Color.rgb, Color.a * Opacity);
}