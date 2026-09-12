#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Render/Renderer.h"

class FFontTexture
{
public:
	FFontTexture();
	~FFontTexture() = default;
	bool LoadTexture(char const* filename);
	void SetSamplerDesc();
	ID3D11ShaderResourceView* GetTextureSRV();
	D3D11_SAMPLER_DESC* GetSamplerDesc();
	float GetWidth() const { return Width; };
	float GetHeight() const { return Height; };

	void Init(FRenderer* InRenderer);
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TextureSRV = nullptr;
	D3D11_SAMPLER_DESC SamplerStateDesc;
	int Width;
	int Height;
	FRenderer* Renderer;
};