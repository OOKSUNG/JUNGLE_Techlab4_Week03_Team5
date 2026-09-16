#include "EnginePCH.h"
#include "Actor.h"

#include "../ObjectSystem/ObjectFactory.h"
#include "../Engine/World.h"

// 컴포넌트 헤더를 따로 만들어야 할까?
#include "../Component/CubeComponent.h"
#include "../Component/SphereComponent.h"
#include "../Component/ConeComponent.h"
#include "../Component/PlaneComponent.h"
#include "../Component/ParticleSubUVComponent.h"
#include "../Engine/ResourceManager.h"

AActor::AActor()
{
	
}

AActor::AActor(FString InName) : Super(InName)
{

}

AActor::~AActor()
{
	//for (UActorComponent* Component : Components)
	//	delete Component;
	//Components.clear();
	//RootComponent = nullptr;   // Root 는 Components에 이미 들어있으므로 delete 하지 말 것
}

void AActor::BeginPlay()
{
	if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(RootComponent))
	{
		World->AddPrimitive(Cast<UPrimitiveComponent>(RootComponent));
	}

	for (UActorComponent* Component : Components)
	{
		Component->BeginPlay();
	}

}

void AActor::Tick(float DeltaTime)
{
	for (UActorComponent* Component : Components)
	{
		Component->TickComponent(DeltaTime);
	}
}

UPrimitiveComponent* AActor::AddPrimitiveComponent(EPrimitiveType Type, FTransform Transform)
{
	UPrimitiveComponent* NewComp = nullptr;
	switch (Type)
	{
	case EPrimitiveType::Sphere:
		NewComp = FObjectFactory::ConstructObject<USphereComponent>();
		break;
	case EPrimitiveType::Cube:
		NewComp = FObjectFactory::ConstructObject<UCubeComponent>();
		break;
	case EPrimitiveType::Cone:
		NewComp = FObjectFactory::ConstructObject<UConeComponent>();
		break;
	case EPrimitiveType::Plane:
		NewComp = FObjectFactory::ConstructObject<UPlaneComponent>();
		break;
	case EPrimitiveType::UVPlane:
		NewComp = FObjectFactory::ConstructObject<UParticleSubUVComponent>();
		break;
	default:
		break;
	break;
	}
	if (NewComp)
	{
		NewComp->SetType(Type);
		NewComp->SetOwner(this);
		NewComp->SetTransform(Transform);

		RootComponent = NewComp;
		Components.push_back(RootComponent);
	}
	return (NewComp);
}

void AActor::SetRootComponent(USceneComponent* SceneComponent)
{
	RootComponent = SceneComponent;
}

USceneComponent* AActor::GetRootComponent()
{
	return RootComponent;
}