// World Transform Matrix
cbuffer constants : register(b0)
{
    matrix MVP;
    uint bHighlightEdge;
    float3 EdgeColor;
};

// View Mode Constants
cbuffer viewModeConstants : register(b1)
{
     uint ViewMode;
     float3 _Pad;
};

struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR; 
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
	float4 color : COLOR;
};

PS_INPUT mainVS(VS_INPUT input)
{
	PS_INPUT output;
    
    output.position = mul(float4(input.position, 1.0f), MVP); // 행벡터 연산임 but 
    //output.position = float4(input.position, 1.0f);   //셰이더는 행렬의 메모리 구조가 다르므로 전치한 결과가 돼서 한번더 전치해야함
    output.color = float4(input.position, 1.0f);
	return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    if (ViewMode == 2)  // WireFrame
    {   
        // 선택하면 형광 주황
        if (bHighlightEdge) return float4(EdgeColor, 1.0f);
        // 형광 초록
        return float4(0.25f, 0.80f, 0.35f, 1.0f);
    }

    // 기본 색상에서 살짝 밝게 처리  + 0.0~1.0 clamp 
    float4 BaseColor = saturate(input.color + float4(0.5f, 0.5f, 0.5f, 0.0f));
    
    if (ViewMode == 0)  // Lit
    {
        // TODO: 조명 구현 후 라이팅 연산으로 교체. 현재는 Unlit과 동일
        return BaseColor;
    }

    return BaseColor;
}