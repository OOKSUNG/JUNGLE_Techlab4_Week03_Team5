#pragma once
#include "Component/PrimitiveComponent.h"
#include "Camera/CameraComponent.h"

class UParticleSubUVComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(UParticleSubUVComponent, UPrimitiveComponent)

    REFLECT_START(ClassName)
        REFLECT_END()

public:
    UParticleSubUVComponent() = default;
    virtual ~UParticleSubUVComponent() = default;

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime) override;
    virtual void SubmitToRenderQueue(TQueue<FRenderPacket>& RenderQueue) override;

    void SetAtlasInfo(int InCols, int InRows, float InFPS);
    void SetTexture(ID3D11ShaderResourceView* InSRV, ID3D11SamplerState* InSampler);
    void SetBillboard(bool bEnable) { bUseBillboard = bEnable; }
    void SetLoop(bool bLoop) { this->bLoop = bLoop; }
    void SetPlayRate(float PlayRate) { this->PlayRate = PlayRate; }

    bool GetLoop() { return bLoop; }
    float GetPlayRate() { return PlayRate; }

private:
    ID3D11ShaderResourceView* TextureSRV = nullptr;
    ID3D11SamplerState* SamplerState = nullptr;

    int Cols = 4;
    int Rows = 4;
    float FPS = 10.0f;
    float PlayRate = 1.0f;
    float ElapsedTime = 0.0f;
    int CurrentFrame = 0;

    FVector2 UVOffset = FVector2(0.0f, 0.0f);
    FVector2 UVScale = FVector2(1.0f, 1.0f);
    bool bLoop = true;
    bool bUseBillboard = true;
};