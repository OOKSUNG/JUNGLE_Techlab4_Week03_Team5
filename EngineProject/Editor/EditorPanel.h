#pragma once

#include <imgui.h>
#include "EditorContext.h"

class IEditorPanel
{
public:
	virtual ~IEditorPanel() = default;

	virtual bool Init() = 0;
	virtual void Tick(float DeltaTime) = 0;
	virtual void OnRender() = 0;

	inline void SetContext(FEditorContext InContext) { Context = InContext; };
	inline FEditorContext GetContext() { return Context; }

protected:
	inline static FEditorContext Context{};
};