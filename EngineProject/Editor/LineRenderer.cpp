#include "EnginePCH.h"
#include "LineRenderer.h"


bool FLineRenderer::Init(FRenderer* Renderer)
{
	VertexBuffer = MakeShared<FDynamicVertexBuffer>(Renderer->GetDevice(), 1000, sizeof(FVertex));
	IndexBuffer = MakeShared<FDynamicIndexBuffer>(Renderer->GetDevice(), 2000);
    ConstantBuffer = Renderer->CreateConstantBuffer(sizeof(LineData));

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION" , 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    LineShader = Renderer->CreateShader(L"Shader/ShaderLine.hlsl", layout, 2);

    return true;
}


void FLineRenderer::AddLine(const FVector& Start, const FVector& End, const FVector4& Color)
{
    uint32 BaseIndex = static_cast<uint32>(Vertices.size());

    Vertices.push_back({ Start, Color });
    Vertices.push_back({ End, Color });

    Indices.push_back(BaseIndex);
    Indices.push_back(BaseIndex + 1);
}

bool FLineRenderer::Flush(FRenderer* Renderer,FVector CameraPos,  FMatrix VP)
{
    if (!Vertices.empty())
    {
        if (!VertexBuffer->Update(Renderer->GetDeviceContext(), Vertices.data(), static_cast<uint32>(sizeof(FVertex)* Vertices.size())))
        {
            return false;
        }
    }

    if (!Indices.empty())
    {
        if (!IndexBuffer->Update(Renderer->GetDeviceContext(), Indices.data(), static_cast<uint32>(Indices.size())))
        {
            return false;
        }
    }

    // Bind
    Renderer->BindShader(LineShader.get());

    // 3. Vertex / Index Buffer
    Renderer->BindVertexBuffer(VertexBuffer.get());
    Renderer->BindIndexBuffer(IndexBuffer.get());
    
    Renderer->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
     
    // Constant Buffer
    LineData Data{};
    Data.ViewProj = VP.GetTransposed();
    Data.CameraPos = CameraPos;
    Data.FadeStart = FadeStart;
    Data.FadeEnd = FadeEnd;

    Renderer->UpdateConstantBufferData(ConstantBuffer.get(), &Data, sizeof(LineData));
    Renderer->BindConstantBuffer(0, ConstantBuffer.get(), EShaderBindFlagBits::Vertex);
    Renderer->BindConstantBuffer(0, ConstantBuffer.get(), EShaderBindFlagBits::Pixel);

    // DrawIndexed()
    Renderer->DrawIndexed(static_cast<uint32>(Indices.size()));

    Vertices.clear();
    Indices.clear();
}

void FLineRenderer::DebugDraw()
{
    // X축 - 빨강
    AddLine(
        FVector(0.0f, 0.0f, 100.0f),
        FVector(100.0f, 0.0f, 0.0f),
        FVector4(0.3f, 0.5f, 0.0f, 1.0f)
    );

    // Y축 - 초록
    AddLine(
        FVector(0.0f, 0.0f, 100.0f),
        FVector(0.0f, 100.0f, 0.0f),
        FVector4(0.0f, 1.0f, 0.3f, 1.0f)
    );

    // Z축 - 파랑
    AddLine(
        FVector(0.0f, 100.0f, 0.0f),
        FVector(0.0f, 0.0f, 100.0f),
        FVector4(0.1f, 0.6f, 0.2f, 1.0f)
    );

}