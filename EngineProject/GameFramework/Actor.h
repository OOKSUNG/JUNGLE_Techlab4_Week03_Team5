#pragma once

#include "../ObjectSystem/Object.h"
#include "Component/PrimitiveComponent.h"
#include "ObjectSystem/Casts.h"
#include "ObjectSystem/Class.h"

class UWorld;
class UTextComponent;

class AActor : public UObject
{
	DECLARE_CLASS(AActor, UObject)

	REFLECT_START(ClassName)
		REFLECT_END()
public:
	AActor();
	AActor(FString InName);
	virtual ~AActor();

	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);

	UWorld* GetWorld() const { return World; }

	//template <typename T>
	//T* AddComponent()
	//{
	//	T* newComponent = 
	//}

	UPrimitiveComponent* GetPrimitiveComponent() const { return Cast<UPrimitiveComponent>(RootComponent); }
	UTextComponent* GetTextComponent() const;
	UPrimitiveComponent* AddPrimitiveComponent(EPrimitiveType Type, FTransform Transform);
	void SetRootComponent(USceneComponent* SceneComponent);
	USceneComponent* GetRootComponent();

	friend class UWorld;
protected:
	TArray<UActorComponent*> Components;

private:
	UWorld* World = nullptr;

	USceneComponent* RootComponent = nullptr;

	// 임시
	//UPrimitiveComponent* Primitive = nullptr;
};