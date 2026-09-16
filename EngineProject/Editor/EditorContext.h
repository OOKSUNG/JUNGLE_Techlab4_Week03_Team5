#pragma once

#include "../Engine/World.h"
#include "Gizmo.h"
#include "EditorSetting.h"

struct FEditorContext
{
	UWorld* World = nullptr;
	FGizmo* Gizmo = nullptr;
	FEditorSettings* EditorSettings = nullptr;
};
