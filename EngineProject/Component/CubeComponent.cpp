#include "EnginePCH.h"
#include "CubeComponent.h"
#include "../Engine/ResourceManager.h"

void UCubeComponent::BeginPlay()
{
	Super::BeginPlay();
	SetMesh(FResourceManager::GetInstance().GetMesh("Cube"));
	SetMeshData(FGeometryGenerator::GetMeshData("Cube"));
	SetFName("CubeComponent_" + std::to_string(GetUUID()));
}
