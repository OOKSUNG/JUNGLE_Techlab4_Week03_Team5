#include "EnginePCH.h"
#include "FontTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../ThirdParty/stb_image/stb_image.h"

FFontTexture::FFontTexture()
{
}

void FFontTexture::Init()
{
}

// TODO:: 용도에 따라 텍스처 로드 방식 다르게 하기
bool FFontTexture::LoadTexture(FRenderer* InRenderer, char const* filename)
{
	Renderer = InRenderer;
	ID3D11Device* Device = Renderer->GetDevice();
	unsigned char* Image = stbi_load(filename, &Width, &Height, nullptr, 4);
	// TODO:: 이미지 파일 오픈 실패 에러 핸들링
	if (!Image)
	{
		std::cout << "현재 작업 디렉터리: " << std::filesystem::current_path() << std::endl;
		// stb_image가 제공하는 실패 에러 메시지 출력
		const char* ErrorReason = stbi_failure_reason();
		printf("stbi_load 실패 사유: %s\n", ErrorReason);
		// 예: "can't fopen", "bad png sig", "unknown image type" 등
	}

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = Image;
	InitData.SysMemPitch = Width*4;
	InitData.SysMemSlicePitch = 0;

	D3D11_TEXTURE2D_DESC TexDesc = {};
	TexDesc.Width = Width;
	TexDesc.Height = Height;
	TexDesc.MipLevels = 1;
	TexDesc.ArraySize = 1;
	TexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	TexDesc.SampleDesc.Count = 1;
	TexDesc.SampleDesc.Quality = 0;
	TexDesc.Usage = D3D11_USAGE_DEFAULT;
	TexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TexDesc.CPUAccessFlags = 0;
	TexDesc.MiscFlags = 0;

	HRESULT hr = Renderer->GetDevice()->CreateTexture2D(&TexDesc, &InitData, Texture.GetAddressOf());
	if (FAILED(hr))
	{
		return FALSE;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = TexDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = 1;
	hr = Device->CreateShaderResourceView(Texture.Get(), &SRVDesc, TextureSRV.GetAddressOf());
	return hr;
}

void FFontTexture::SetSamplerState()
{
	D3D11_SAMPLER_DESC SamplerStateDesc;
	SamplerStateDesc = {};
	SamplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	SamplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerStateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerStateDesc.MinLOD = 0;
	SamplerStateDesc.MaxLOD = D3D11_FLOAT32_MAX;

	Renderer->GetDevice()->CreateSamplerState(&SamplerStateDesc, SamplerState.GetAddressOf());
}

void FFontTexture::SetParticleSamplerState()
{
	D3D11_SAMPLER_DESC SamplerStateDesc;
	SamplerStateDesc = {};
	SamplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerStateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerStateDesc.MinLOD = 0;
	SamplerStateDesc.MaxLOD = D3D11_FLOAT32_MAX;

	Renderer->GetDevice()->CreateSamplerState(&SamplerStateDesc, SamplerState.GetAddressOf());
}


ID3D11SamplerState* FFontTexture::GetSamplerState()
{
	return (SamplerState.Get());
}


ID3D11ShaderResourceView* FFontTexture::GetTextureSRV()
{
	return (TextureSRV.Get());
}