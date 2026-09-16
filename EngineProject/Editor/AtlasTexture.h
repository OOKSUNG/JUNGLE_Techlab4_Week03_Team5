#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Render/Renderer.h"

class FAtlasTexture
{
public:
	FAtlasTexture();
	~FAtlasTexture() = default;
	bool LoadTexture(FRenderer* InRenderer, char const* filename);
	void SetSamplerState();
	void SetParticleSamplerState();
	ID3D11ShaderResourceView* GetTextureSRV();
	ID3D11SamplerState* GetSamplerState();
	float GetWidth() const { return Width; };
	float GetHeight() const { return Height; };

	void Init();
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TextureSRV = nullptr;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> SamplerState;
	int Width;
	int Height;
	FRenderer* Renderer;
};