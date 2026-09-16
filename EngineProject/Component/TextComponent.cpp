#include "EnginePCH.h"
#include "TextComponent.h"

void UTextComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTextComponent::UpdateBounds()
{
    FTransform* T = GetTransform();
    float ScaleRight = T->Scale.Y * WorldScaleFactor;
    float ScaleUp    = T->Scale.Z * WorldScaleFactor;

    FVector HW = T->GetRight() * (LocalHalfWidth  * ScaleRight);
    FVector HU = T->GetUp()    * (LocalHalfHeight * ScaleUp);
    FVector Loc = T->Location;

    FVector Corners[4] = { Loc + HW + HU, Loc + HW - HU, Loc - HW + HU, Loc - HW - HU };

    FVector Mn = Corners[0], Mx = Corners[0];
    for (const FVector& C : Corners)
    {
        Mn.X = (C.X < Mn.X) ? C.X : Mn.X;  Mx.X = (C.X > Mx.X) ? C.X : Mx.X;
        Mn.Y = (C.Y < Mn.Y) ? C.Y : Mn.Y;  Mx.Y = (C.Y > Mx.Y) ? C.Y : Mx.Y;
        Mn.Z = (C.Z < Mn.Z) ? C.Z : Mn.Z;  Mx.Z = (C.Z > Mx.Z) ? C.Z : Mx.Z;
    }

    Bounds.Origin = (Mn + Mx) * 0.5f;
    Bounds.BoxExtent = (Mx - Mn) * 0.5f;
}