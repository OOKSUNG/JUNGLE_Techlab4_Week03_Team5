#include "EnginePCH.h"
#include "ParticleSubUVComponent.h"
#include "Engine/ResourceManager.h"

#include "GameFrameWork/Actor.h"
#include "Engine/World.h"

void UParticleSubUVComponent::BeginPlay()
{
	Super::BeginPlay();
	FMesh* UVMesh = FResourceManager::GetInstance().GetMesh("UVPlane");
	FShader* UVShader = FResourceManager::GetInstance().GetShader("Shader/SubUVShader.hlsl");
	SetMeshShader(UVMesh, UVShader);

	SetMeshData(FGeometryGenerator::GetMeshData("UVPlane"));
}

void UParticleSubUVComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);

    ElapsedTime += DeltaTime *PlayRate;
    int TotalFrames = Cols * Rows;
    int DesiredFrame = static_cast<int>(ElapsedTime * FPS);
    if (bLoop)
    {
        CurrentFrame = DesiredFrame % TotalFrames;
    }
    else
    {
        if (DesiredFrame >= TotalFrames)
            CurrentFrame = TotalFrames - 1;
        else
            CurrentFrame = DesiredFrame;
    }

     float CellW = 1.0f / Cols;
     float CellH = 1.0f / Rows;
     UVOffset.X = (CurrentFrame % Cols) * CellW;
     UVOffset.Y = (CurrentFrame / Cols) * CellH;
     UVScale = FVector2(CellW, CellH);
   
}

void UParticleSubUVComponent::SubmitToRenderQueue(TQueue<FRenderPacket>& RenderQueue)
{
    if (!GetVisible() || !GetMesh() || !TextureSRV) return;

    FRenderPacket Packet;
    Packet.mesh = GetMesh();
    Packet.shader = GetShader();
    Packet.bIsVisible = GetVisible();
    Packet.Owner = this;


    if (bUseBillboard && GetWorld() && GetWorld()->GetMainCamera())
    {
        UCameraComponent* Cam = GetWorld()->GetMainCamera()->GetCameraComponent();
        FMatrix CamWorld = Cam->GetWorldMatrix();

        FVector CamRight = FVector(CamWorld[1][0], CamWorld[1][1], CamWorld[1][2]).Normalize();
        FVector CamUp = FVector(CamWorld[2][0], CamWorld[2][1], CamWorld[2][2]).Normalize();
        FVector CamForward = (CamRight.Cross(CamUp)).Normalize();

        FVector Scale = GetTransform()->Scale;
        FMatrix BillboardMat;
        BillboardMat.SetIdentity();
        BillboardMat[0][0] = CamRight.X * Scale.X;   BillboardMat[0][1] = CamRight.Y * Scale.X;   BillboardMat[0][2] = CamRight.Z * Scale.X;
        BillboardMat[1][0] = CamUp.X * Scale.Y;      BillboardMat[1][1] = CamUp.Y * Scale.Y;      BillboardMat[1][2] = CamUp.Z * Scale.Y;
        BillboardMat[2][0] = -CamForward.X * Scale.Z; BillboardMat[2][1] = -CamForward.Y * Scale.Z; BillboardMat[2][2] = -CamForward.Z * Scale.Z;

        FVector Pos = GetTransform()->Location;
        BillboardMat[3][0] = Pos.X; BillboardMat[3][1] = Pos.Y; BillboardMat[3][2] = Pos.Z;

        Packet.model = BillboardMat;
    }
    else
    {
        Packet.model = GetTransform()->GetWorldMatrix();
    }


    Packet.TextureSRV = TextureSRV;
    Packet.SamplerState = SamplerState;
    Packet.UVOffset = UVOffset;
    Packet.UVScale = UVScale;

    RenderQueue.push(Packet);
}

void UParticleSubUVComponent::SetAtlasInfo(int InCols, int InRows, float InFPS)
{
    Cols = InCols;
    Rows = InRows;
    FPS = InFPS;
    ElapsedTime = 0.0f;
}

void UParticleSubUVComponent::SetTexture(ID3D11ShaderResourceView* InSRV, ID3D11SamplerState* InSampler)
{
    TextureSRV = InSRV;
    SamplerState = InSampler;
}
