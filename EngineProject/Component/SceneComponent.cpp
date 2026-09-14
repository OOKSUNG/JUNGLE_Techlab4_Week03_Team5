#include "EnginePCH.h"
#include "Component/SceneComponent.h"

FMatrix USceneComponent::GetWorldMatrix() const
{
    FMatrix RotationMatrix = transform.Rotation.Quaternion().ToFMatrix();

    FMatrix WorldMatrix = FMatrix(
        RotationMatrix[0][0] * transform.Scale.X,
        RotationMatrix[0][1] * transform.Scale.X,
        RotationMatrix[0][2] * transform.Scale.X,
        0.0f,

        RotationMatrix[1][0] * transform.Scale.Y,
        RotationMatrix[1][1] * transform.Scale.Y,
        RotationMatrix[1][2] * transform.Scale.Y,
        0.0f,

        RotationMatrix[2][0] * transform.Scale.Z,
        RotationMatrix[2][1] * transform.Scale.Z,
        RotationMatrix[2][2] * transform.Scale.Z,
        0.0f,

        transform.Location.X,
        transform.Location.Y,
        transform.Location.Z,
        1.0f
    );

    return WorldMatrix;
}

FTransform* USceneComponent::GetTransform()
{
    return &transform;
}

const FBoxBounds& USceneComponent::GetBounds() const
{
    return Bounds;
}

void USceneComponent::SetTransform(FTransform Transform)
{
    transform = Transform;
    UpdateBounds();
}

void USceneComponent::SetLocation(FVector NewLocation)
{
    transform.Location = NewLocation;
    UpdateBounds();
}

void USceneComponent::SetScale(FVector NewScale)
{
    transform.Scale = NewScale;
    UpdateBounds();
}

void USceneComponent::SetRotation(FRotator NewRotation)
{
    transform.Rotation = NewRotation;
    UpdateBounds();
}