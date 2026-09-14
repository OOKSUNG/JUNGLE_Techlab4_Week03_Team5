#pragma once
#include "Math/EngineMath.h"
#include <d3d11.h>
#include "FontAtlas.h"
#include "Render/Renderer.h"


struct FTextVertex
{
    FVector2 LocalOffset;
    FVector2 TexCoord;
};

class FTextRenderer
{
public:
    bool Init(FRenderer* Renderer, const wchar_t* FontPath, int FontPixelSize, int AtlasSize = 1024);
    void RenderText(FRenderer* Renderer, const FString& Utf8Text, FVector2 ScreenOffset, FVector4 Color, uint32 ScreenWidth, uint32 ScreenHeight);
    inline bool SaveAtlasDebugBMP(FRenderer* Renderer, const char* FilePath) const { return Atlas.SaveDebugBMP(Renderer, FilePath); }
    
private:
    TArray<FTextVertex> BuildTextQuads(const FString& Utf8Text, ID3D11DeviceContext* Context);
    TArray<uint32> BuildQuadIndices(size_t VertexCount);

    FDynamicFontAtlas Atlas;

    TSharedPtr<FShader> TextShader;
    TSharedPtr<FDynamicVertexBuffer> VB;
    TSharedPtr<FDynamicIndexBuffer> IB;
    TSharedPtr<FConstantBuffer> CB;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> Sampler;
    Microsoft::WRL::ComPtr<ID3D11BlendState> BlendState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> NoCullState;

    static constexpr uint32 MaxVertices = 4096;
    static constexpr uint32 MaxIndices = 6144;
    
};

