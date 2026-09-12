cbuffer LineData : register(b0)
{
    float4x4 ViewProj;
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float4 Color : COLOR;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

VS_OUTPUT mainVS(VS_INPUT Input)
{
    VS_OUTPUT Output;

    // Row Vector 방식
    Output.Position = mul(float4(Input.Position, 1.0f), ViewProj);

    Output.Color = Input.Color;

    return Output;
}

float4 mainPS(VS_OUTPUT Input) : SV_TARGET
{
    return Input.Color;
}