#pragma once
#include "../Render/Renderer.h"
#include "../Render/Buffer.h"

struct LineData
{
    FMatrix ViewProj;

    //
    FVector CameraPos;
    float FadeStart;
    float FadeEnd;
    float Padding[3] = {};
};

class FLineRenderer
{
public:
    bool Init(FRenderer* Renderer);

    void AddLine( const FVector& Start, const FVector& End, const FVector4& Color);

    bool Flush(FRenderer* Renderer, FVector CameraPos, FMatrix VP);

    void DebugDraw();
    void DrawBox(const FBoxBounds& Bounds);

private:

    TSharedPtr<FDynamicVertexBuffer> VertexBuffer = nullptr;
    TSharedPtr<FDynamicIndexBuffer> IndexBuffer = nullptr;

    TSharedPtr<FConstantBuffer> ConstantBuffer = nullptr;

    TSharedPtr<FShader> LineShader;

    // ID3D11VertexShader* VertexShader = nullptr;
    // ID3D11PixelShader* PixelShader = nullptr;
    // ID3D11InputLayout* InputLayout = nullptr;

    TArray<FVertex> Vertices;
    TArray<uint32> Indices;


    float FadeStart = 800.0f;
    float FadeEnd = 1000.0f;

};

