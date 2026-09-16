#pragma once
#include "../GameFramework/Actor.h"
#include "GridRenderer.h"
#include "GizmoRenderer.h"
#include "Outline.h"
#include "OutLineRenderer.h"
#include "BoundingBox.h"

class FEditorSelection
{
public:
	void Select(AActor* Actor);
	void Clear();
	
	// bool 
	// void Update();

	AActor* GetSelectedActor() const;


private:
	AActor* PickedActor;
	UPrimitiveComponent* PickedComponent;

	TUniquePtr<FGizmoRenderer> GizmoRenderer;
	TSharedPtr<FGizmo> Gizmo;
	TUniquePtr<FOutline> Outline;
	TUniquePtr<FOutlineRenderer> OutlineRenderer;
	TUniquePtr<FBoundingBox> BoundingBox;
};

