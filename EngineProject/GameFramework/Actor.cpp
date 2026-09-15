#include "EnginePCH.h"
#include "Actor.h"

#include "../ObjectSystem/ObjectFactory.h"
#include "../Engine/World.h"

// 컴포넌트 헤더를 따로 만들어야 할까?
#include "../Component/CubeComponent.h"
#include "../Component/SphereComponent.h"
#include "../Component/ConeComponent.h"
#include "../Component/PlaneComponent.h"
#include "../Component/TextComponent.h"
#include "../Engine/ResourceManager.h"

AActor::AActor()
{
	
}

AActor::~AActor()
{
	//for (UActorComponent* Component : Components)
	//	delete Component;
	//Components.clear();
	//RootComponent = nullptr;   // Root 는 Components에 이미 들어있으므로 delete 하지 말 것
}

UTextComponent* AActor::GetTextComponent() const
{
	return Cast<UTextComponent>(RootComponent);
}

void AActor::BeginPlay()
{
	
	if (UTextComponent* TextComp = Cast<UTextComponent>(RootComponent))
	{
		World->AddTextComponent(TextComp);
	}
	else if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(RootComponent))
	{
		World->AddPrimitive(Primitive);
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

void AActor::AddPrimitiveComponent(EPrimitiveType Type, FTransform Transform)
{
	switch (Type)
	{
	case EPrimitiveType::Sphere:
		RootComponent = FObjectFactory::ConstructObject<USphereComponent>();
		Cast<UPrimitiveComponent>(RootComponent)->SetType(Type);
		break;
	case EPrimitiveType::Cube:
		RootComponent = FObjectFactory::ConstructObject<UCubeComponent>();
		Cast<UPrimitiveComponent>(RootComponent)->SetType(Type);
		break;
	case EPrimitiveType::Cone:
		RootComponent = FObjectFactory::ConstructObject<UConeComponent>();
		Cast<UPrimitiveComponent>(RootComponent)->SetType(Type);
		break;
	case EPrimitiveType::Plane:
		RootComponent = FObjectFactory::ConstructObject<UPlaneComponent>();
		Cast<UPrimitiveComponent>(RootComponent)->SetType(Type);
		break;
	case EPrimitiveType::Text:
		RootComponent = FObjectFactory::ConstructObject<UTextComponent>();
		Cast<UTextComponent>(RootComponent)->SetType(Type);
		break;
	default:
		break;
	break;
	}
	RootComponent->SetTransform(Transform);
	Components.push_back(RootComponent);
}

void AActor::SetRootComponent(USceneComponent* SceneComponent)
{
	RootComponent = SceneComponent;
}

USceneComponent* AActor::GetRootComponent()
{
	return RootComponent;
}