#include "EnginePCH.h"
#include "PrimitiveComponent.h"
#include "../Render/Renderer.h"
#include "../Engine/ResourceManager.h"

void UPrimitiveComponent::BeginPlay()
{
	Super::BeginPlay();
	Shader = FResourceManager::GetInstance().GetShader("Shader/DefaultShader.hlsl");
}

void UPrimitiveComponent::SubmitToRenderQueue(TQueue<FRenderPacket>& RenderQueue)
{
	if (Mesh && Shader)
	{
		FRenderPacket rp;
		rp.mesh = Mesh;
		rp.shader = Shader;
		rp.model = GetWorldMatrix();
		rp.bIsVisible = bIsVisible;
		rp.Owner = this;  // WireFrame 모드 시, 선택 객체 판별용
		RenderQueue.push(rp);
	}

}

void UPrimitiveComponent::UpdateBounds()
{
	Super::UpdateBounds();
	const FMatrix& WorldMatrix = GetWorldMatrix();
	const FMeshData& Mesh = GetMeshData();
	FVector BoxMin, BoxMax;
	Mesh.GetWorldAABB(BoxMin, BoxMax, WorldMatrix);
	Bounds.Origin = (BoxMax + BoxMin) / 2.0f;
	Bounds.BoxExtent = (BoxMax - BoxMin) / 2.0f;
}

void UPrimitiveComponent::SetMesh(FMesh* InMesh)
{
	Mesh = InMesh;
}
