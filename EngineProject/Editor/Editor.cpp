#include "EnginePCH.h"
#include "Editor.h"
#include "../Camera/CameraComponent.h"
#include "Input/InputSystem.h"
#include "Editor/PropertyPanel.h"
#include "Editor/ControlPanel.h"
#include "EditorContext.h"
#include "FontRenderer.h"
#include "FontManager.h"

bool FEditor::Init(FRenderer* InRenderer ,UWorld* World, HWND hwnd)
{
	Gizmo = MakeUnique<FGizmo>();
	Outline = MakeUnique<FOutline>();

	ImGuiRenderer = MakeUnique<FImGuiRenderer>();
	if (!ImGuiRenderer->Init(hwnd, InRenderer->GetDevice(), InRenderer->GetDeviceContext()))
	{
		// LOG(Error, "Failed To Initialize ImGui!");
		return false;
	}

	// UI가 사용할 정보 저장
	Context.World = World;
	Context.Gizmo = Gizmo.get();



	// UI 생성 및 초기화, 컨텍스트 전달
	EditorUI = MakeUnique<FEditorUI>();

	if (!EditorUI->Init(Context))
	{
		return false;
	}
	ConsolePanel = EditorUI->GetEditorPanel<FConsolePanel>();
	ControlPanel = EditorUI->GetEditorPanel<FControlPanel>();
	ControlPanel->SetRenderer(InRenderer);

	ControlPanel->ShowFlags = &GetShowFlags();
	ControlPanel->TestFont();
	// 씬 클리어 호출 시 콜백 함수
	ControlPanel->SetSceneClearCallback([&]() {
		Gizmo->SetTarget(nullptr);
		Outline->SetTarget(nullptr);
		EditorUI->GetEditorPanel<FPropertyPanel>()->SetTarget(nullptr);
		ShowFlags.SetDefault();
		}
	);

	GridRenderer = MakeUnique<FGridRenderer>();
	GridRenderer->Init(InRenderer);

	GizmoRenderer = MakeUnique<FGizmoRenderer>();
	GizmoRenderer->Init(InRenderer);

	OutlineRenderer = MakeUnique<FOutlineRenderer>();
	OutlineRenderer->Init(InRenderer);

	FontRenderer = MakeUnique<FFontRenderer>();
	FontRenderer->Init(InRenderer);

	FFontManager::GetIntance().Init(InRenderer);
	FFontManager::GetIntance().LoadFontAtlas("Default", "Font//Default", 16, 16);
}

void FEditor::Update(float DeltaTime, UCameraComponent* Camera, FMatrix VP, uint32 WinWidth, uint32 WinHeight)
{
	EditorUI->Tick(DeltaTime);

	FRay ray = Camera->DeProjection(FInputSystem::GetMouseX(), FInputSystem::GetMouseY(), WinWidth, WinHeight);
	FVector2 mousePos(FInputSystem::GetMouseX(), FInputSystem::GetMouseY());
	bool bMouseDown = FInputSystem::IsMouseDown(EMouseButton::Left);

	Gizmo->Update(ray, mousePos, VP, WinWidth, WinHeight, bMouseDown, Camera);

	if (FInputSystem::IsMousePressed(EMouseButton::Left) && !Gizmo->IsUsing() && Gizmo->GetHoveredAxis() < 0 && !ImGui::GetIO().WantCaptureMouse)
	{
		UPrimitiveComponent* PickedComponent = Context.World->GetPickingPrimitive(WinWidth, WinHeight);
		Gizmo->SetTarget(PickedComponent);
		Outline->SetTarget(PickedComponent);
		EditorUI->GetEditorPanel<FPropertyPanel>()->SetTarget(PickedComponent);
	}
}

void FEditor::OnRender(FMatrix VP, UCameraComponent* Camera, FRenderer* Renderer)
{
	const FVector CamLoc = Camera->GetLocation();
	
	if (ShowFlags.IsSet(EShowFlagBits::Grid))
		GridRenderer->OnRender(VP, CamLoc);

	if (Outline->GetTarget() && ShowFlags.IsSet(EShowFlagBits::OutLine) && ShowFlags.IsSet(EShowFlagBits::Primitives)
	&& Renderer->GetViewMode() != EViewModeIndex::Wireframe)
		OutlineRenderer->OnRender(*Outline, VP, CamLoc);	

	if (Gizmo->GetTarget() && ShowFlags.IsSet(EShowFlagBits::Gizmo))
	{
		Renderer->SetDepthStencilEnabled(false);
		GizmoRenderer->OnRender(*Gizmo, VP);
	}



	ImGuiRenderer->Begin();

	EditorUI->OnRender();

	ImGuiRenderer->End();
}

void FEditor::Shutdown()
{
	ImGuiRenderer->Shutdown();
}