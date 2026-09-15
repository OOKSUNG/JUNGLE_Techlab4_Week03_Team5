#pragma once
#include "../GameFramework/Actor.h"


class FEditorSelection
{
public:
	void Select(AActor* Actor);
	void Clear();

	AActor* GetSelectedActor() const;

};

