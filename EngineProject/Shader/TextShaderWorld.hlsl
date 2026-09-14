Texture2D AtlasTex : register(t0);
SamplerState AtlasSampler : register(s0);

cbuffer TextDataWorld : register(b0)
{
    matrix VP;
    float4 Color;
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

float4 mainPS(VS_OUTPUT Input) : SV_TARGET
{
    float Alpha = AtlasTex.Sample(AtlasSampler, Input.TexCoord).r;
    return float4(Color.rgb, Color.a * Alpha);
}