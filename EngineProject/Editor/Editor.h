#pragma once

#include "Render/Renderer.h"
#include "ImGuiRenderer.h"
#include "GridRenderer.h"
#include "GizmoRenderer.h"

#include "EditorUI.h"
#include "ConsolePanel.h"
#include "ControlPanel.h"


#include "Outline.h"
#include "OutLineRenderer.h"
#include "ShowFlags.h"

#include "LineRenderer.h"

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

	void DrawGrid(const FVector& CameraPos);
	void AxisDraw();

	static FConsolePanel* GetConsolePanel() { return ConsolePanel; }
private:
	TUniquePtr<FImGuiRenderer> ImGuiRenderer;
	//TUniquePtr<FGridRenderer> GridRenderer;
	TUniquePtr<FGizmoRenderer> GizmoRenderer;
	TSharedPtr<FGizmo> Gizmo;
	TUniquePtr<FOutline> Outline;
	TUniquePtr<FOutlineRenderer> OutlineRenderer;
	FEngineShowFlags ShowFlags;

	TUniquePtr<FEditorUI> EditorUI;
	FEditorContext Context;

	TUniquePtr<FLineRenderer> LineRenderer;

	inline static FConsolePanel* ConsolePanel = nullptr;
	inline static FControlPanel* ControlPanel = nullptr;

	float GridExtent = 1000.0f;
	float GridSpacing = 10.0f;
	int32 GridCount = 20;

};

