#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class FTexture2D
{
public:
	FTexture2D(ID3D11Device* Device, const D3D11_TEXTURE2D_DESC& Desc);
	~FTexture2D();

	uint32 GetWidth() const { return Width; }
	uint32 GetHeight() const { return Height; }
	ID3D11Texture2D* GetTexture() { return Texture2D.Get(); }
private:
	uint32 Width;
	uint32 Height;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
};