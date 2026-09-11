#pragma once

class UWorld;
class FGizmo;

struct FEditorContext
{
	UWorld* World = nullptr;
	FGizmo* Gizmo = nullptr;
};
