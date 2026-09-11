#pragma once

#include "../Engine/World.h"
#include "Gizmo.h"

struct FEditorContext
{
	UWorld* World = nullptr;
	FGizmo* Gizmo = nullptr;
};
