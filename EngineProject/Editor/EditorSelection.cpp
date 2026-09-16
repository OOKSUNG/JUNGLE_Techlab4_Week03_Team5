#include "EnginePCH.h"
#include "EditorSelection.h"




void FEditorSelection::Select(AActor* Actor)
{
	Clear();

	UPrimitiveComponent* PickedComponent = Cast<UPrimitiveComponent>(Actor->GetRootComponent());

	if (!PickedComponent) return;

	// Editor->SetTarget(PickedComponent);
}

void FEditorSelection::Clear()
{

	


}
//
//AActor* FEditorSelection::GetSelectedActor() const
//{
//	
//}

//void FEditorSelection::SetTarget(UPrimitiveComponent* PickedComponent)
//{
//	Gizmo->SetTarget(PickedComponent);
//	Outline->SetTarget(PickedComponent);
//	BoundingBox->SetTarget(PickedComponent);
//	EditorUI->GetEditorPanel<FPropertyPanel>()->SetTarget(PickedComponent);
//}
//
//void FEditorSelection::SetSceneClear()
//{
//	Gizmo->SetTarget(nullptr);
//	Outline->SetTarget(nullptr);
//	BoundingBox->SetTarget(nullptr);
//	EditorUI->GetEditorPanel<FPropertyPanel>()->SetTarget(nullptr);
//	ShowFlags.SetDefault();
//	PickedComponent = nullptr;
//}