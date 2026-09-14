Texture2D AtlasTex : register(t0);
SamplerState AtlasSampler : register(s0);

cbuffer TextData : register(b0)
{
    float2 ScreenOffset;    // 텍스트 시작 위치
    float2 ScreenSize;      // 뷰포트 크기
    float4 Color;
    float PxRange;
    float3 Pad;
};

struct VS_INPUT
{
    float2 LocalOffset : POSITION;
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
    float2 PixelPos = Input.LocalOffset + ScreenOffset;
    float2 NDC = PixelPos / (ScreenSize * 0.5f);
    Output.Position = float4(NDC, 0.0f, 1.0f);
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
    float ScreenTexSize = 1.0f / fwidth(Input.TexCoord);
    float ScreenPxRange = max(0.5f * dot(UnitRange, ScreenTexSize), 1.0f);

    float Opacity = saturate(ScreenPxRange * Sd + 0.5f);
    return float4(Color.rgb, Color.a * Opacity);
}

