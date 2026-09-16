#pragma once
#include "../GameFramework/Actor.h"


class FEditorSelection
{
public:
	void Select(AActor* Actor);
	void Clear();

	void Update();

	AActor* GetSelectedActor() const;


private:
	AActor* PickedActor;
	UPrimitiveComponent* PickedComponent;
};

