#include "EnginePCH.h"
#include "ActorComponent.h"

#include "GameFrameWork/Actor.h"
#include "Engine/World.h"

void UActorComponent::SetOwner(AActor* InOwner)
{ 
    Owner = InOwner; 
}
AActor* UActorComponent::GetOwner() const
{
    return Owner; 
}

UWorld* UActorComponent::GetWorld() const
{
    return Owner ? Owner->GetWorld() : nullptr;
}