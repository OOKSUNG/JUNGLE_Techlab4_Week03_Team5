#include "EnginePCH.h"
#include "ConeComponent.h"
#include "../Engine/ResourceManager.h"

void UConeComponent::BeginPlay()
{
	Super::BeginPlay();
	SetMesh(FResourceManager::GetInstance().GetMesh("Cone"));
	SetMeshData(FGeometryGenerator::GetMeshData("Cone"));
	SetFName("ConeComponent_" + std::to_string(GetUUID()));
}
