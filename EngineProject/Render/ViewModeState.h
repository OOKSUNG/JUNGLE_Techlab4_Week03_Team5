#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Core/Types.h"
#include "Render/Buffer.h"

class FRenderer;

enum class EViewModeIndex : uint32
{
    Lit,
    Unlit,
    Wireframe,
};

struct FViewModeConstants
{
    uint32 ViewMode = 0;
    float Padding[3] = {};  // 16바이트 정렬용
};

class FViewModeState
{
public:
    explicit FViewModeState(FRenderer* Renderer);
    ~FViewModeState() = default;

    inline void SetMode(EViewModeIndex Mode) { CurrentMode = Mode; }
    inline EViewModeIndex GetMode() const { return CurrentMode; }

    void Apply(FRenderer* Renderer);

private:
    EViewModeIndex CurrentMode = EViewModeIndex::Unlit; // 기본값은 Unlit ( 조명 없이 텍스처의 color만 )

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> SolidRasterizerState;     // 면 채우기
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> WireframeRasterizerState; // 선 채우기

    TSharedPtr<FConstantBuffer> ConstantBuffer;

};