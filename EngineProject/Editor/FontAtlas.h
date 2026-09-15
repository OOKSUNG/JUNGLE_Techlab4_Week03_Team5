#pragma once
#include "FontTexture.h"
#include <wrl/client.h>
#include "Render/Renderer.h"

struct FFontUV
{
	float minU, minV;
	float maxU, maxV;
};

// 해당 클래스에서 fonttexture 호출해서 SRV 생성, GetSRV, C 입력 시 UV 반환
class FFontAtlas
{
public:
	FFontAtlas();
	~FFontAtlas() = default;
	void Init(FRenderer* InRenderer, char const* FileName, int Cols, int Rows);
	ID3D11ShaderResourceView* GetTextureSRV();
	ID3D11SamplerState* GetSamplerState();
	FFontUV GetAtlasUV(char C);
private:
	FFontTexture FontTexture;
	int Cols;
	int Rows;
	float FontWidth;
	float FontHeight;
};