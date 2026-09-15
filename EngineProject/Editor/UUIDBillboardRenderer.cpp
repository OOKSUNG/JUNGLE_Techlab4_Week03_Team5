#include "EnginePCH.h"
#include "UUIDBillboardRenderer.h"

void FUUIDBillboardRenderer::Init(UWorld* World)
{
	this->World = World;
}

void FUUIDBillboardRenderer::SetUUIDTextItemList(UCameraComponent* Camera)
{
	UUIDTextItemList.clear();
	FVector CamPos = Camera->GetLocation();
	for (UPrimitiveComponent* Primitive : World->GetPrimitiveComponents())
	{
		if (!Primitive || !Primitive->GetVisible()) continue;
		FString Text = "UUID: " + std::to_string(Primitive->GetUUID());
		FVector WorldStartPos = Primitive->GetTransform()->Location + FVector(0.5f);
		float DistSqr = FVector::DistanceSqr(CamPos, WorldStartPos);
		FMatrix CamWorldMat = Camera->GetWorldMatrix();
		FVector UpVector = FVector(CamWorldMat[2][0], CamWorldMat[2][1], CamWorldMat[2][2]).Normalize();
		FVector RightVector = FVector(CamWorldMat[1][0], CamWorldMat[1][1], CamWorldMat[1][2]).Normalize();
		UUIDTextItemList.push_back({ Text, WorldStartPos, RightVector, UpVector, DistSqr, 1.0f });
	}
	std::sort(UUIDTextItemList.begin(), UUIDTextItemList.end(), [](const FWorldTextItem& A, const FWorldTextItem& B) {return A.DistSqr > B.DistSqr;});
}