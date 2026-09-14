#pragma once
#include "EditorPanel.h"
#include "EditorContext.h"


class FContentBrowser : IEditorPanel
{
public:
	bool Init() override;
	void Tick(float DeltaTime)override;
	void OnRender() override;

	inline void SetContext(FEditorContext InContext) { Context = InContext; }
	inline FEditorContext GetContext() { return Context; }

private:
	FEditorContext Context;

};

