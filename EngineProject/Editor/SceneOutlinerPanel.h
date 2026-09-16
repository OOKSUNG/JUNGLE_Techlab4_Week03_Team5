#pragma once
#include "EditorPanel.h"
#include "EditorContext.h"


class FSceneOutlinerPanel : public IEditorPanel
{
public:
	bool Init() override;
	void Tick(float DeltaTime)override;
	void OnRender() override;

	inline void SetActorDeleteCallback(std::function<void(AActor*)> InCallback) { ActorDeleteCallback = InCallback; }

	std::function<void(AActor*)> ActorDeleteCallback = nullptr;

	//inline void SetContext(FEditorContext InContext) { Context = InContext; }
	//inline FEditorContext GetContext() { return Context; }

	inline AActor* GetSelectedActor() { return SelectedActor; }
	inline void SetSelectedActor(AActor* Actor) { SelectedActor = Actor; }

private:
	//FEditorContext Context;
	AActor* SelectedActor = nullptr;

	AActor* RenameTarget = nullptr;
	char RenameBuffer[256] = {};

	std::function<void(AActor*)> OnDeleteActor;
};

