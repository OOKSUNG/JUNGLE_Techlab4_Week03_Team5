#include "EnginePCH.h"
#include "TextRenderer.h"
#include "StringUtils.h"

bool FTextRenderer::Init(FRenderer* Renderer, const wchar_t* FontPath, int FontPixelSize, int AtlasSize)
{
    if (!Atlas.Init(Renderer->GetDevice(), FontPath, FontPixelSize, AtlasSize))
        return false;
    
    
    D3D11_INPUT_ELEMENT_DESC Layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    TextShader = Renderer->CreateShader(L"Shader/TextShader.hlsl", Layout, 2);
    if (!TextShader)
        return false;

    VB = MakeShared<FDynamicVertexBuffer>(Renderer->GetDevice(), MaxVertices, static_cast<uint32>(sizeof(FTextVertex)));
    IB = MakeShared<FDynamicIndexBuffer>(Renderer->GetDevice(), MaxIndices);
    CB = Renderer->CreateConstantBuffer(sizeof(float) * 12); // ScreenOffset+ScreenSize+Color+PxRange+Pad

    // Sampler 명세
    D3D11_SAMPLER_DESC SamplerDesc = {};
    SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    if (FAILED(Renderer->GetDevice()->CreateSamplerState(&SamplerDesc, &Sampler)))
        return false;

    // Blend 명세
    D3D11_BLEND_DESC BlendDesc = {};
    BlendDesc.RenderTarget[0].BlendEnable = TRUE;
    BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    if (FAILED(Renderer->GetDevice()->CreateBlendState(&BlendDesc, &BlendState)))
        return false;

    // Rasterizer 명세
    D3D11_RASTERIZER_DESC RasterDesc = {};
    RasterDesc.FillMode = D3D11_FILL_SOLID;
    RasterDesc.CullMode = D3D11_CULL_NONE;

    if (FAILED(Renderer->GetDevice()->CreateRasterizerState(&RasterDesc, &NoCullState)))
        return false;
    

    
    D3D11_INPUT_ELEMENT_DESC WorldLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    TextShaderWorld = Renderer->CreateShader(L"Shader/TextShaderWorld.hlsl", WorldLayout, 2);
    if (!TextShaderWorld)
        return false;

    VBWorld = MakeShared<FDynamicVertexBuffer>(Renderer->GetDevice(), MaxVertices, static_cast<uint32>(sizeof(FTextVertexWorld)));
    IBWorld = MakeShared<FDynamicIndexBuffer>(Renderer->GetDevice(), MaxIndices);
    CBWorld = Renderer->CreateConstantBuffer(sizeof(float) * 24); // VP (16) + Color (4) + PxRange + Pad(3)


    return true;
}

TArray<FTextVertex> FTextRenderer::BuildTextQuads(const FString& Utf8Text, ID3D11DeviceContext* Context)
{
    TArray<FTextVertex> Vertices;
    TArray<uint32> Codepoints = DecodeUTF8(Utf8Text);

    float PenX = 0.0f; // 현재까지 그린 x 위치 (좌->우)

    for (uint32 Codepoint : Codepoints)
    {
        const FGlyphInfo& Glyph = Atlas.GetOrCreateGlyph(Codepoint, Context);

        float X0 = PenX + Glyph.BearingX;
        float Y0 = static_cast<float>(Glyph.BearingY);
        float X1 = X0 + Glyph.BitmapWidth;
        float Y1 = Y0 - Glyph.BitmapHeight;

        Vertices.push_back({ {X0, Y0}, {Glyph.U,                Glyph.V} });                // 좌상
        Vertices.push_back({ {X1, Y0}, {Glyph.U + Glyph.Width,  Glyph.V} });                // 우상
        Vertices.push_back({ {X0, Y1}, {Glyph.U,                Glyph.V + Glyph.Height} }); // 좌하
        Vertices.push_back({ {X1, Y1}, {Glyph.U + Glyph.Width,  Glyph.V + Glyph.Height} }); // 우하

        PenX += Glyph.Advance;
    }
    return Vertices;

}

TArray<uint32> FTextRenderer::BuildQuadIndices(size_t VertexCount)
{
    TArray<uint32> Indices;
    uint32 QuadCount = static_cast<uint32>(VertexCount / 4);
    for (uint32 i = 0; i < QuadCount; i++)
    {
        uint32 Base = i * 4;
        Indices.push_back(Base + 0); Indices.push_back(Base + 1); Indices.push_back(Base + 2);
        Indices.push_back(Base + 2); Indices.push_back(Base + 1); Indices.push_back(Base + 3);
    }
    return Indices;
}

TArray<FTextVertexWorld> FTextRenderer::BuildTextQuadsWorld(const FString& Utf8Text, ID3D11DeviceContext* Context, const FVector& WorldPosition, const FVector& Right, const FVector& Up, float Scale)
{
    TArray<FTextVertexWorld> Vertices;
    TArray<uint32> Codepoints = DecodeUTF8(Utf8Text);

    float PenX = 0.0f;

    for (uint32 Codepoint : Codepoints)
    {
        const FGlyphInfo& Glyph = Atlas.GetOrCreateGlyph(Codepoint, Context);

        float X0 = (PenX + Glyph.BearingX) * Scale;
        float Y0 = static_cast<float>(Glyph.BearingY) * Scale;
        float X1 = X0 + Glyph.BitmapWidth * Scale;
        float Y1 = Y0 - Glyph.BitmapHeight * Scale;
        
        auto ToWorld = [&](float LocalX, float LocalY) -> FVector
        {
            return WorldPosition + Right * LocalX + Up * LocalY;
        };

        Vertices.push_back({ ToWorld(X0, Y0), {Glyph.U,               Glyph.V                }});
        Vertices.push_back({ ToWorld(X1, Y0), {Glyph.U + Glyph.Width, Glyph.V                }});
        Vertices.push_back({ ToWorld(X0, Y1), {Glyph.U,               Glyph.V + Glyph.Height }});
        Vertices.push_back({ ToWorld(X1, Y1), {Glyph.U + Glyph.Width, Glyph.V + Glyph.Height }});

        PenX += Glyph.Advance;

    }
    return Vertices;
}

void FTextRenderer::RenderText(FRenderer* Renderer, const FString& Utf8Text, FVector2 ScreenOffset, FVector4 Color, uint32 ScreenWidth, uint32 ScreenHeight)
{
    ID3D11DeviceContext* Context = Renderer->GetDeviceContext();

    TArray<FTextVertex> Vertices = BuildTextQuads(Utf8Text, Context);
    if (Vertices.empty())
        return;
    
    TArray<uint32> Indices = BuildQuadIndices(Vertices.size());

    if (!VB->Update(Context, Vertices.data(), static_cast<uint32>(sizeof(FTextVertex) * Vertices.size())))
        return;

    if (!IB->Update(Context, Indices.data(), static_cast<uint32>(Indices.size())))
        return;

    struct FTextCBData
    {
        float ScreenOffset[2];
        float ScreenSize[2];
        float Color[4];
        float PxRange;
        float Pad[3];
    } CBData;

    CBData.ScreenOffset[0] = ScreenOffset.X;
    CBData.ScreenOffset[1] = ScreenOffset.Y;
    CBData.ScreenSize[0] = static_cast<float>(ScreenWidth);
    CBData.ScreenSize[1] = static_cast<float>(ScreenHeight);
    CBData.PxRange = Atlas.GetPxRange();

    CBData.Color[0] = Color.X;
    CBData.Color[1] = Color.Y;
    CBData.Color[2] = Color.Z;
    CBData.Color[3] = Color.W;

    Renderer->UpdateConstantBufferData(CB.get(), &CBData, sizeof(CBData));
        
    float BlendFactor[4] = { 0,0,0,0 };
    Context->OMSetBlendState(BlendState.Get(), BlendFactor, 0xffffffff);
    Renderer->SetDepthStencilEnabled(false);
    Context->RSSetState(NoCullState.Get());

    Renderer->BindShader(TextShader.get());
    Renderer->BindVertexBuffer(VB.get());
    Renderer->BindIndexBuffer(IB.get());
    Renderer->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Renderer->BindConstantBuffer(0, CB.get(), EShaderBindFlagBits::Vertex);
    Renderer->BindConstantBuffer(0, CB.get(), EShaderBindFlagBits::Pixel);

    ID3D11ShaderResourceView* AtlasSRV = Atlas.GetAtlasSRV();
    ID3D11SamplerState* SamplerRaw = Sampler.Get();
    Context->PSSetShaderResources(0, 1, &AtlasSRV);
    Context->PSSetSamplers(0, 1, &SamplerRaw);

    Renderer->DrawIndexed(static_cast<uint32>(Indices.size()));

    Renderer->SetDepthStencilEnabled(true);
    Context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
}

void FTextRenderer::RenderTextWorld(FRenderer* Renderer, const FString& Utf8Text, const FVector& WorldPosition, const FVector& CameraRight, const FVector& CameraUp, float Scale, FVector4 Color, const FMatrix& VP)
{
    ID3D11DeviceContext* Context = Renderer->GetDeviceContext();
    TArray<FTextVertexWorld> Vertices = BuildTextQuadsWorld(Utf8Text, Context, WorldPosition, CameraRight, CameraUp, Scale);
    if (Vertices.empty())
        return;

    TArray<uint32> Indices = BuildQuadIndices(Vertices.size());

    if (!VBWorld->Update(Context, Vertices.data(), static_cast<uint32>(sizeof(FTextVertexWorld) * Vertices.size())))
        return;
    if (!IBWorld->Update(Context, Indices.data(), static_cast<uint32>(Indices.size())))
        return;
    
    struct FTextCBDataWorld
    {
        float VP[16];
        float Color[4];
        float PxRange;
        float Pad[3];
    } CBData;

    FMatrix TransposedVP = VP.GetTransposed();
    memcpy(CBData.VP, &TransposedVP, sizeof(CBData.VP));
    
    CBData.Color[0] = Color.X;
    CBData.Color[1] = Color.Y;
    CBData.Color[2] = Color.Z;
    CBData.Color[3] = Color.W;
    CBData.PxRange = Atlas.GetPxRange();

    Renderer->UpdateConstantBufferData(CBWorld.get(), &CBData, sizeof(CBData));

    float BlendFactor[4] = { 0,0,0,0 };

    // 0xffffffff -> 전체 샘플에 다 적용
    Context->OMSetBlendState(BlendState.Get(), BlendFactor, 0xffffffff);
    // Test On, Write Off
    Context->OMSetDepthStencilState(Renderer->GetDepthTestOnlyState(), 0);  
    Context->RSSetState(NoCullState.Get());

    Renderer->BindShader(TextShaderWorld.get());
    Renderer->BindVertexBuffer(VBWorld.get());
    Renderer->BindIndexBuffer(IBWorld.get());
    Renderer->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Renderer->BindConstantBuffer(0, CBWorld.get(), EShaderBindFlagBits::Vertex);
    Renderer->BindConstantBuffer(0, CBWorld.get(), EShaderBindFlagBits::Pixel);

    ID3D11ShaderResourceView* AtlasSRV = Atlas.GetAtlasSRV();
    ID3D11SamplerState* SamplerRaw = Sampler.Get();
    Context->PSSetShaderResources(0, 1, &AtlasSRV);
    Context->PSSetSamplers(0, 1, &SamplerRaw);

    Renderer->DrawIndexed(static_cast<uint32>(Indices.size()));
    // Wtire On (다음 오브젝트를 위해 정상 depth 상태로 복귀)
    Renderer->SetDepthStencilEnabled(true);
    Context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
}
