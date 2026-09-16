cbuffer FSubUVConstants : register(b0)
{
    float4x4 MVP;
    int bHighlightEdge;
    float3 EdgeColor;
    float2 UVOffset;
    float2 UVScale;
};

Texture2D SubUVTexture : register(t0);
SamplerState SubUVSampler : register(s0);

struct VS_INPUT
{
    float3 Position : POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
};

PS_INPUT mainVS(VS_INPUT Input)
{
    PS_INPUT Output;
    Output.Position = mul(float4(Input.Position, 1.0f), MVP);
    Output.Color = Input.Color;

    Output.TexCoord = (Input.TexCoord * UVScale) + UVOffset;

    return Output;
}

float4 mainPS(PS_INPUT Input) : SV_TARGET
{
    float4 TexColor = SubUVTexture.Sample(SubUVSampler, Input.TexCoord);
    clip(TexColor.a - 0.01f);


    // float Alpha = TexColor.a; // 또는 TexColor.r
    return TexColor * Input.Color;
    //return float4(TexColor.rgb, 1.0f);
}