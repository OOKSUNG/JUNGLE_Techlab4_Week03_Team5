#pragma once

#include "Render/Renderer.h"
#include "ImGuiRenderer.h"
#include "GridRenderer.h"
#include "GizmoRenderer.h"
#include "../Text/TextRenderer.h"
#include "FontRenderer.h"
#include "UUIDBillboardRenderer.h"

#include "EditorUI.h"
#include "ConsolePanel.h"
#include "ControlPanel.h"


#include "Outline.h"
#include "OutLineRenderer.h"
#include "ShowFlags.h"
#include "BoundingBox.h"
#include "LineRenderer.h"
#include "EditorFileUtils.h"

#include "EditorSetting.h"

class FEditor
{

public:
	bool Init(FRenderer* InRenderer, UWorld* World, HWND hwnd);
	void Update(float DeltaTime, UCameraComponent* Camera, FMatrix VP, uint32 WinWidth, uint32 WinHeight);
	void OnRender(FMatrix VP, UCameraComponent* Camera, FRenderer* InRenderer);

	// OnRender에서 분리한 함수 (Gizmo)
	void RenderGizmo(FMatrix VP, FRenderer* InRenderer);

	void Shutdown();

	void SetTarget(UPrimitiveComponent* PickedComponent);
	void SetSceneClear();

	FEngineShowFlags& GetShowFlags() { return ShowFlags; };
	const FEngineShowFlags& GetShowFlags() const { return ShowFlags; };
	UPrimitiveComponent* GetSelectedTarget() const { return Outline->GetTarget(); }
	FEditorSettings* GetEditorSettings() const { return EditorSettings.get(); }

	void DrawGrid(const FVector& CameraPos);
	void AxisDraw();

	void SetPickedComponent(UPrimitiveComponent* Component) { PickedComponent = Component; }

	void ClearSceneTargetsAndFlags();

	static FConsolePanel* GetConsolePanel() { return ConsolePanel; }


	void ClearActorReference(AActor* Actor);

	void DeleteActor(AActor* Actor);

private:
	TUniquePtr<FImGuiRenderer> ImGuiRenderer;
	TUniquePtr<FGizmoRenderer> GizmoRenderer;
	TSharedPtr<FGizmo> Gizmo;
	TUniquePtr<FOutline> Outline;
	TUniquePtr<FOutlineRenderer> OutlineRenderer;
	TUniquePtr<FBoundingBox> BoundingBox;
	FEngineShowFlags ShowFlags;

	TUniquePtr<FEditorUI> EditorUI;
	FEditorContext Context;

	TUniquePtr<FEditorFileUtils> EditorFileUtils;

	TUniquePtr<FLineRenderer> LineRenderer;

	TUniquePtr<FTextRenderer> TextRenderer;
	TUniquePtr<FUUIDBillboardRenderer> UUIDBillboardRenderer;
	TUniquePtr<FFontRenderer> FontRenderer;

	inline static FConsolePanel* ConsolePanel = nullptr;
	inline static FControlPanel* ControlPanel = nullptr;

	TUniquePtr<FEditorSettings> EditorSettings;

	float GridExtent = 1000.0f;
	// float GridSpacing = 10.0f;
	int32 GridCount = 20;

	AActor* PickedActor;
	UPrimitiveComponent* PickedComponent;
};

