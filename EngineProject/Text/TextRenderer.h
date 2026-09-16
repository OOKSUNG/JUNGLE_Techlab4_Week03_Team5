#pragma once
#include "Math/EngineMath.h"
#include <d3d11.h>
#include "FontAtlas.h"
#include "Render/Renderer.h"
#include "../Core/Types.h"

class UTextComponent;

struct FTextVertex
{
    FVector2 LocationOffet;   
    FVector2 TexCoord;
};

struct FTextVertexWorld
{
    FVector Position;   // 월드 좌표 (빌보드 계산 완료)
    FVector2 TexCoord;
};

class FTextRenderer
{

using FTextVertexPageMap = TMap<int, TArray<FTextVertexWorld>>;

public:
    bool Init(FRenderer* Renderer, const wchar_t* FontPath, int FontPixelSize, int AtlasSize = 1024);
    void RenderText(FRenderer* Renderer, const FString& Utf8Text, FVector2 ScreenOffset, FVector4 Color, uint32 ScreenWidth, uint32 ScreenHeight);
    
    void RenderTextWorld(FRenderer* Renderer, const FString& Utf8Text, const FVector& WorldPosition,
                        const FVector& Right, const FVector& Up, float ScaleX, float ScaleY,
                        FVector4 Color, const FMatrix& VP, FDynamicFontAtlas& InAtlas);
    
    void RenderTextComponents(const TArray<UTextComponent*>& TextComponents, FRenderer* Renderer, const FMatrix& VP);
    
    // Bounding Box만 계산하는 함수
    void UpdateTextComponentBounds(const TArray<UTextComponent*>& TextComponents, ID3D11DeviceContext* Context);

    inline bool SaveAtlasDebugBMP(FRenderer* Renderer, const char* FilePath) const { return Atlas.SaveDebugBMP(Renderer, FilePath); }
    
private:
    TArray<FTextVertex> BuildTextQuads(const FString& Utf8Text, ID3D11DeviceContext* Context);
    FTextVertexPageMap BuildTextQuadsWorld(const FString& Utf8Text, ID3D11DeviceContext* Context,
                                                const FVector& WorldPosition, const FVector& Right,
                                                const FVector& Up, float ScaleX,  float ScaleY, FDynamicFontAtlas& InAtlas);
    TArray<uint32> BuildQuadIndices(size_t VertexCount);

    // Caching & Batching
    void DrawTextPageMap(FRenderer* Renderer, const FTextVertexPageMap& PageVertices, FVector4 Color,
                        const FMatrix& VP, FDynamicFontAtlas& InAtlas);
    void DrawTextBatch(FRenderer* Renderer, const TArray<FTextVertexWorld>& Vertices, FVector4 Color,
                        const FMatrix& VP, FDynamicFontAtlas& InAtlas, int PageIndex);

    FDynamicFontAtlas Atlas;
    
    // Component 별 마지막으로 빌드한 vertex 캐시
    struct FTextRenderCache
    {
        FTextVertexPageMap PageVertices;
        FVector LastLocation;
        FVector LastRight;
        FVector LastUp;
        float LastScaleRight = 0.0f;
        float LastScaleUp = 0.0f;
    };

    TMap<UTextComponent*, FTextRenderCache> ComponentVertexCache;

    TSharedPtr<FShader> TextShader;
    TSharedPtr<FDynamicVertexBuffer> VB;
    TSharedPtr<FDynamicIndexBuffer> IB;
    TSharedPtr<FConstantBuffer> CB;

    TSharedPtr<FShader> TextShaderWorld;
    TSharedPtr<FDynamicVertexBuffer> VBWorld;
    TSharedPtr<FDynamicIndexBuffer> IBWorld;
    TSharedPtr<FConstantBuffer> CBWorld;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> Sampler;
    Microsoft::WRL::ComPtr<ID3D11BlendState> BlendState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> NoCullState;

    static constexpr uint32 MaxVertices = 4096;
    static constexpr uint32 MaxIndices = 6144;    
};

