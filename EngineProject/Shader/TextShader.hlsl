Texture2D AtlasTex : register(t0);
SamplerState AtlasSampler : register(s0);

cbuffer TextData : register(b0)
{
    float2 ScreenOffset;    // 텍스트 시작 위치
    float2 ScreenSize;      // 뷰포트 크기
    float4 Color;
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

float4 mainPS(VS_OUTPUT Input) : SV_TARGET
{
    float Alpha = AtlasTex.Sample(AtlasSampler, Input.TexCoord).r;
    return float4(Color.rgb, Color.a * Alpha);
    // float4 Tex = AtlasTex.Sample(AtlasSampler, Input.TexCoord);
    // return float4(Tex.rrr, 1.0);   // 알파 무시, R값을 흑백으로 강제 출력 (항상 불투명)
}

