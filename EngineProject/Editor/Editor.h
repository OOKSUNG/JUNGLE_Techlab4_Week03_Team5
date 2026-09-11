#pragma once

#include "Render/Renderer.h"
#include "ImGuiRenderer.h"
#include "GridRenderer.h"
#include "GizmoRenderer.h"

#include "EditorUI.h"
#include "ConsolePanel.h"


#include "Outline.h"
#include "OutLineRenderer.h"

class FEditor
{

public:
	bool Init(FRenderer* InRenderer, UWorld* World, HWND hwnd);
	void Update(float DeltaTime, UCameraComponent* Camera, FMatrix VP, uint32 WinWidth, uint32 WinHeight);
	void OnRender(FMatrix VP, UCameraComponent* Camera, FRenderer* InRenderer);
	void Shutdown();

	static FConsolePanel* GetConsolePanel() { return ConsolePanel; }
private:
	TUniquePtr<FImGuiRenderer> ImGuiRenderer;
	TUniquePtr<FGridRenderer> GridRenderer;
	TUniquePtr<FGizmoRenderer> GizmoRenderer;
	//TUniquePtr<FGizmo> Gizmo;
	TSharedPtr<FGizmo> Gizmo;
	TUniquePtr<FOutline> Outline;
	TUniquePtr<FOutlineRenderer> OutlineRenderer;

	TUniquePtr<FEditorUI> EditorUI;
	FEditorContext Context;

	inline static FConsolePanel* ConsolePanel = nullptr;

};

