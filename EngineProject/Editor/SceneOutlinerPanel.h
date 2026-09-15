#pragma once
#include "EditorPanel.h"
#include "EditorContext.h"


class FSceneOutlinerPanel : public IEditorPanel
{
public:
	bool Init() override;
	void Tick(float DeltaTime)override;
	void OnRender() override;

	inline void SetContext(FEditorContext InContext) { Context = InContext; }
	inline FEditorContext GetContext() { return Context; }

	inline AActor* GetSelectedActor() { return SelectedActor; }

private:
	FEditorContext Context;
	AActor* SelectedActor = nullptr;

};

