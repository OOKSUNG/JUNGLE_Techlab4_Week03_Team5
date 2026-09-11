#include "EnginePCH.h"
#include "PlaneComponent.h"
#include "../Engine/ResourceManager.h"

void UPlaneComponent::BeginPlay()
{
	Super::BeginPlay();
	SetMesh(FResourceManager::GetInstance().GetMesh("Plane"));
	SetMeshData(FGeometryGenerator::GetMeshData("Plane"));
}