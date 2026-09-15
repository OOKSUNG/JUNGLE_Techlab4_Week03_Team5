#include "EnginePCH.h"
#include "FontRenderer.h"

bool FFontRenderer::Init(FRenderer* InRenderer)
{
	Renderer = InRenderer;
	ID3D11Device* Device = Renderer->GetDevice();

	CB = Renderer->CreateConstantBuffer(sizeof(FFontData));
	
	D3D11_BUFFER_DESC VBDesc = {};
	VBDesc.ByteWidth = sizeof(FFontVertex) * MaxTextCount * 4;
	VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Device->CreateBuffer(&VBDesc, nullptr, (ID3D11Buffer**)DynamicVB.GetAddressOf());

	D3D11_DEPTH_STENCIL_DESC DSDesc = {};
	DSDesc.DepthEnable = TRUE;
	DSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DSDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	Device->CreateDepthStencilState(&DSDesc, DepthReadOnlyState.GetAddressOf());

	TArray<UINT> Indices;
	Indices.reserve(MaxTextCount * 6);
	for (UINT i = 0; i < MaxTextCount; ++i)
	{
		UINT BaseV = i * 4;
		Indices.push_back(BaseV + 0);
		Indices.push_back(BaseV + 1);
		Indices.push_back(BaseV + 2);
		Indices.push_back(BaseV + 0);
		Indices.push_back(BaseV + 2);
		Indices.push_back(BaseV + 3);
	}
	StaticIB = Renderer->CreateIndexBuffer(Indices.data(), (UINT)Indices.size());

	D3D11_INPUT_ELEMENT_DESC Layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	Shader = Renderer->CreateShader(L"Shader/FontShader.hlsl", Layout, 2);

	D3D11_RASTERIZER_DESC RasterizerDesc = {};
	RasterizerDesc.FillMode = D3D11_FILL_SOLID;
	RasterizerDesc.CullMode = D3D11_CULL_BACK;
	Renderer->GetDevice()->CreateRasterizerState(&RasterizerDesc, &RasterizerState);
	
	D3D11_BLEND_DESC BlendDesc = {};
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	Device->CreateBlendState(&BlendDesc, &AlphaBlendState);

	return true;
}

void FFontRenderer::Begin()
{

}

void FFontRenderer::End()
{

}

void FFontRenderer::Shutdown()
{

}

void FFontRenderer::RenderBatchTexts(TArray<FWorldTextItem> TextItemArray, UCameraComponent* Camera)
{
	if (TextItemArray.empty()) return;
	FontTexture = FFontManager::GetIntance().FontTextureMap["Default"].get();
	ID3D11DeviceContext* DeviceContext = Renderer->GetDeviceContext();
	TArray<FFontVertex> Vertices;
	for (const FWorldTextItem& TextItem : TextItemArray)
	{
		float Distance = sqrt(TextItem.DistSqr);
		float CharWidth = 32.0f * 0.002f * Distance * TextItem.Scale;
		float HeightHalf = CharWidth * 0.5f;

		FVector Cursor = TextItem.WorldStartPos;


		for (char C : TextItem.Text)
		{
			if (Vertices.size() / 4 >= MaxTextCount) break;

			uint8 Index = static_cast<int>(C);
			float CellSize = 1.0f / 16.0f;
			
			float UMin = (Index % 16) * CellSize;
			float VMin = (Index / 16) * CellSize;
			float UMax = UMin + CellSize;
			float VMax = VMin + CellSize;

			FVector V0 = Cursor + (TextItem.CamUp * HeightHalf);
			FVector V1 = Cursor + (TextItem.CamUp * HeightHalf) + (TextItem.CamRight * CharWidth);
			FVector V2 = Cursor + (TextItem.CamUp * -HeightHalf) + (TextItem.CamRight * CharWidth);
			FVector V3 = Cursor + (TextItem.CamUp * -HeightHalf);
			
			Vertices.push_back({ V0, FVector2(UMin, VMin) });
			Vertices.push_back({ V1, FVector2(UMax, VMin) });
			Vertices.push_back({ V2, FVector2(UMax, VMax) });
			Vertices.push_back({ V3, FVector2(UMin, VMax) });
			Cursor += TextItem.CamRight * CharWidth;
		}
	}
	if (Vertices.empty()) return;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	if (SUCCEEDED(DeviceContext->Map((ID3D11Resource*)DynamicVB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)))
	{
		memcpy(MappedResource.pData, Vertices.data(), sizeof(FFontVertex) * Vertices.size());
		DeviceContext->Unmap((ID3D11Resource*)DynamicVB.Get(), 0);
	}

	DeviceContext->RSSetState(RasterizerState.Get());
	DeviceContext->OMSetBlendState(AlphaBlendState.Get(), nullptr, 0xffffffff);

	DeviceContext->OMSetDepthStencilState(DepthReadOnlyState.Get(), 0);

	Renderer->BindShader(Shader.get());
	Renderer->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	FFontData ConstData;
	ConstData.VP = Camera->GetViewProjectionMatrix().GetTransposed();
	ConstData.color = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
	Renderer->UpdateConstantBufferData(CB.get(), &ConstData, sizeof(FFontData));
	Renderer->BindConstantBuffer(0, CB.get(), EShaderBindFlagBits::Vertex);
	Renderer->BindConstantBuffer(0, CB.get(), EShaderBindFlagBits::Pixel);
	
	ID3D11ShaderResourceView* SRV = FontTexture->GetTextureSRV();
	DeviceContext->PSSetShaderResources(0, 1, &SRV);
	ID3D11SamplerState* SS = FontTexture->GetSamplerState();
	DeviceContext->PSSetSamplers(0, 1, &SS);

	UINT Stride = sizeof(FFontVertex);
	UINT Offset = 0;
	ID3D11Buffer* pVB = (ID3D11Buffer*)DynamicVB.Get();
	DeviceContext->IASetVertexBuffers(0, 1, &pVB, &Stride, &Offset);
	Renderer->BindIndexBuffer(StaticIB.get());

	UINT TextCount = (UINT)Vertices.size() / 4;
	Renderer->DrawIndexed(TextCount * 6);

	DeviceContext->RSSetState(nullptr);
	DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);	

	DeviceContext->OMSetDepthStencilState(nullptr, 0);
}