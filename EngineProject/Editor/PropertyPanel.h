#pragma once

#include <format>
#include "EditorPanel.h"
#include "EditorContext.h"
#include "../Component/SceneComponent.h"

struct FTransform;

class FPropertyPanel : public IEditorPanel
{
public:
	FPropertyPanel() = default;
	~FPropertyPanel();

	bool Init() override;
	void Tick(float DeltaTime)override;
	void OnRender() override;
	
	//inline void SetContext(FEditorContext InContext) { Context = InContext; }
	//inline FEditorContext GetContext() { return Context; }

	void SetTarget(USceneComponent* InTarget) { Target = InTarget; }

private:
	//FEditorContext Context;

	//UWorld* World;

	USceneComponent* Target;
};

