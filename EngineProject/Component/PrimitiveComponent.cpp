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

void UPrimitiveComponent::SetMesh(FMesh* InMesh)
{
	Mesh = InMesh;
}
