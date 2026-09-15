#include "EnginePCH.h"
#include "TextComponent.h"

void UTextComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTextComponent::UpdateBounds()
{
	FTransform* Transform = GetTransform();
	FVector Location = Transform->Location;
	FVector Right = Transform->GetRight();
	FVector Up = Transform->GetUp();
	float Scale = Transform->Scale.X * WorldScaleFactor;

	// 대략적인 값 계산
	float HalfWidth = Text.length() * FontPixelSize * 0.5f * Scale;
	float HalfHeight = FontPixelSize * 0.5f * Scale;

	FVector Corners[4] =
	{
		Location + Right * HalfWidth + Up * HalfHeight, // 우상
		Location - Right * HalfWidth + Up * HalfHeight, // 좌상
		Location + Right * HalfWidth - Up * HalfHeight, // 우하
		Location - Right * HalfWidth - Up * HalfHeight, // 좌하
	};

	// x,y,z축 각각 비교하여 가장 작은 값 저장
	FVector BoxMin = Corners[0];
	FVector BoxMax = Corners[0];
	for (int i = 1; i < 4; ++i)
	{
		BoxMin.X = (Corners[i].X < BoxMin.X) ? Corners[i].X : BoxMin.X;
		BoxMin.Y = (Corners[i].Y < BoxMin.Y) ? Corners[i].Y : BoxMin.Y;
		BoxMin.Z = (Corners[i].Z < BoxMin.Z) ? Corners[i].Z : BoxMin.Z;
		BoxMax.X = (Corners[i].X > BoxMax.X) ? Corners[i].X : BoxMax.X;
		BoxMax.Y = (Corners[i].Y > BoxMax.Y) ? Corners[i].Y : BoxMax.Y;
		BoxMax.Z = (Corners[i].Z > BoxMax.Z) ? Corners[i].Z : BoxMax.Z;
	}

	// 중심점 / 반경 찾기
	Bounds.Origin = (BoxMax + BoxMin) * 0.5f;
	Bounds.BoxExtent = (BoxMax - BoxMin) * 0.5f;
}