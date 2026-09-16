cbuffer constants : register(b0)
{
    matrix ViewProjection;
    float4 Color;
};

Texture2D FontAtlasTexture : register(t0);
SamplerState FontSampler : register(s0);

struct VS_INPUT
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    output.position = mul(float4(input.position, 1.0f), ViewProjection);
    output.texcoord = input.texcoord;
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    float4 TexColor = FontAtlasTexture.Sample(FontSampler, input.texcoord);
    float TextAlpha = TexColor.r;
    
    return float4(Color.rgb, Color.a * TextAlpha);
}

