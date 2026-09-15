#pragma once

#include "Render/Renderer.h"
#include "ImGuiRenderer.h"
#include "GridRenderer.h"
#include "GizmoRenderer.h"
#include "FontRenderer.h"
#include "UUIDBillboardRenderer.h"

#include "EditorUI.h"
#include "ConsolePanel.h"
#include "ControlPanel.h"


#include "Outline.h"
#include "OutLineRenderer.h"
#include "ShowFlags.h"

class FEditor
{

public:
	bool Init(FRenderer* InRenderer, UWorld* World, HWND hwnd);
	void Update(float DeltaTime, UCameraComponent* Camera, FMatrix VP, uint32 WinWidth, uint32 WinHeight);
	void OnRender(FMatrix VP, UCameraComponent* Camera, FRenderer* InRenderer);
	void Shutdown();
	FEngineShowFlags& GetShowFlags() { return ShowFlags; };
	const FEngineShowFlags& GetShowFlags() const { return ShowFlags; };
	UPrimitiveComponent* GetSelectedTarget() const { return Outline->GetTarget(); }

	static FConsolePanel* GetConsolePanel() { return ConsolePanel; }
private:
	TUniquePtr<FImGuiRenderer> ImGuiRenderer;
	TUniquePtr<FGridRenderer> GridRenderer;
	TUniquePtr<FGizmoRenderer> GizmoRenderer;
	//TUniquePtr<FGizmo> Gizmo;
	TSharedPtr<FGizmo> Gizmo;
	TUniquePtr<FOutline> Outline;
	TUniquePtr<FOutlineRenderer> OutlineRenderer;
	FEngineShowFlags ShowFlags;

	TUniquePtr<FEditorUI> EditorUI;
	FEditorContext Context;

	TUniquePtr<FUUIDBillboardRenderer> UUIDBillboardRenderer;
	TUniquePtr<FFontRenderer> FontRenderer;

	inline static FConsolePanel* ConsolePanel = nullptr;
	inline static FControlPanel* ControlPanel = nullptr;

};

