#include "EnginePCH.h"
#include "ViewModeState.h"
#include "Renderer.h"

FViewModeState::FViewModeState(FRenderer* Renderer)
{
    // Solid Rasterizer 명세
    ID3D11Device* Device = Renderer->GetDevice();
    D3D11_RASTERIZER_DESC SolidDesc = {};
    SolidDesc.FillMode = D3D11_FILL_SOLID;
    SolidDesc.CullMode = D3D11_CULL_BACK;
    Device->CreateRasterizerState(&SolidDesc, SolidRasterizerState.GetAddressOf());
    
    // WireFrame Rasterizer 명세
    D3D11_RASTERIZER_DESC WireDesc = {};
    WireDesc.FillMode = D3D11_FILL_WIREFRAME;
    WireDesc.CullMode = D3D11_CULL_BACK;
    Device->CreateRasterizerState(&WireDesc, WireframeRasterizerState.GetAddressOf());

    ConstantBuffer = Renderer->CreateConstantBuffer(sizeof(FViewModeConstants));

}

void FViewModeState::Apply(FRenderer* Renderer)
{
    // Binding State(Wire or Solid)
    ID3D11RasterizerState* StateToBind =
    (CurrentMode == EViewModeIndex::Wireframe) ? WireframeRasterizerState.Get() : SolidRasterizerState.Get();

    Renderer->GetDeviceContext()->RSSetState(StateToBind);

    FViewModeConstants Constants;
    Constants.ViewMode = static_cast<uint32>(CurrentMode); // Enum -> int
    Renderer->UpdateConstantBufferData(ConstantBuffer.get(), &Constants, sizeof(Constants));

    // 1번 슬롯에 Pixel Shading용으로 Binding
    Renderer->BindConstantBuffer(1, ConstantBuffer.get(), EShaderBindFlagBits::Pixel);

}