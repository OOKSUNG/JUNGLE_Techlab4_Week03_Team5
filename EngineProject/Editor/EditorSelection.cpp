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