#include "EnginePCH.h"
#include "FontAtlas.h"
#include "FontTexture.h"

FFontAtlas::FFontAtlas()
{

}

void FFontAtlas::Init(FRenderer* InRenderer, char const* FileName, int AtlasCols, int AtlasRows)
{
	FontTexture.Init(InRenderer);
	FontTexture.LoadTexture(FileName);
	FontTexture.SetSamplerDesc();
	Cols = AtlasCols;
	Rows = AtlasRows;
	FontWidth = FontTexture.GetWidth() / Cols;
	FontHeight = FontTexture.GetHeight() / Rows;
}

ID3D11ShaderResourceView* FFontAtlas::GetTextureSRV()
{
	return (FontTexture.GetTextureSRV());
}

D3D11_SAMPLER_DESC* FFontAtlas::GetSamplerDesc()
{
	return (FontTexture.GetSamplerDesc());
}

FFontUV FFontAtlas::GetAtlasUV(char C)
{
	int Index = static_cast<int>(C);
	int Row = Index / Cols;
	int Col = Index % Cols;
	FFontUV FontUV;
	FontUV.minU = Col * FontWidth;
	FontUV.minV = Row * FontHeight;
	FontUV.maxU = FontUV.minU + FontWidth;
	FontUV.maxV = FontUV.minV + FontHeight;
	return(FontUV);
}