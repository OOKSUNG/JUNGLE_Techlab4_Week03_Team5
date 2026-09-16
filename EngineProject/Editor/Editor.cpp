#include "EnginePCH.h"
#include "Editor.h"
#include "../Camera/CameraComponent.h"
#include "Input/InputSystem.h"
#include "Editor/PropertyPanel.h"
#include "Editor/ControlPanel.h"
#include "Editor/SceneOutlinerPanel.h"
#include "EditorContext.h"
#include "Core/FBoxBounds.h"
#include "FontRenderer.h"
#include "AtlasTextureManager.h"
#include "UUIDBillboardRenderer.h"


bool FEditor::Init(FRenderer* InRenderer, UWorld* World, HWND hwnd)
{
	Gizmo = MakeUnique<FGizmo>();
	Outline = MakeUnique<FOutline>();
	BoundingBox = MakeUnique<FBoundingBox>();

	ImGuiRenderer = MakeUnique<FImGuiRenderer>();
	if (!ImGuiRenderer->Init(hwnd, InRenderer->GetDevice(), InRenderer->GetDeviceContext()))
	{
		// LOG(Error, "Failed To Initialize ImGui!");
		return false;
	}

	EditorSettings = MakeUnique<FEditorSettings>();
	EditorSettings->LoadEditorSetting();

	// UI가 사용할 정보 저장
	Context.World = World;
	Context.Gizmo = Gizmo.get();
	Context.EditorSettings = EditorSettings.get();

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

	// 씬 클리어 호출 시 콜백 함수

	/*ControlPanel->SetSceneClearCallback([&]() {
		Gizmo->SetTarget(nullptr);
		Outline->SetTarget(nullptr);
		BoundingBox->SetTarget(nullptr);
		EditorUI->GetEditorPanel<FPropertyPanel>()->SetTarget(nullptr);
		ShowFlags.SetDefault();
		PickedComponent = nullptr;
		}
	);*/

	ControlPanel->SetNewSceneCallback([&]()
		{
			ClearSceneTargetsAndFlags();
			Context.World->ClearScene();
			Context.World->NewScene();
			
		}
	);

	ControlPanel->SetLoadSceneCallback([&]()
		{
			FSceneMetaData SceneData;
			if (EditorFileUtils->LoadSceneFromFileSelection(SceneData))
			{
				ClearSceneTargetsAndFlags();
				Context.World->ClearScene();
				Context.World->LoadScene(SceneData);
				
			}
			else {
				LOG(Editor, Error, "Failed Loading Scene File...");
			}
		}
	);

	ControlPanel->SetSaveSceneCallback([&]()
		{
			FSceneMetaData SceneData;
			if (Context.World->SaveScene(SceneData) && EditorFileUtils->SaveSceneWithFileBrowser(SceneData))
			{
			}
			else {
				LOG(Editor, Error, "Failed Saving Scene File...");
			}
		}
	);

	LineRenderer = MakeUnique<FLineRenderer>();
	LineRenderer->Init(InRenderer);

	GizmoRenderer = MakeUnique<FGizmoRenderer>();
	GizmoRenderer->Init(InRenderer);

	OutlineRenderer = MakeUnique<FOutlineRenderer>();
	OutlineRenderer->Init(InRenderer);

	EditorFileUtils = MakeUnique<FEditorFileUtils>();

	UUIDBillboardRenderer = MakeUnique<FUUIDBillboardRenderer>();
	UUIDBillboardRenderer->Init(Context.World);

	FontRenderer = MakeUnique<FFontRenderer>();
	FontRenderer->Init(InRenderer);

	FAtlasTextureManager::GetIntance().Init(InRenderer);
	FAtlasTextureManager::GetIntance().LoadFontTexture("Default", "Font\\Default.png");
	
	return true;

}

void FEditor::SetTarget(UPrimitiveComponent* PickedComponent)
{
	Gizmo->SetTarget(PickedComponent);
	Outline->SetTarget(PickedComponent);
	BoundingBox->SetTarget(PickedComponent);
	EditorUI->GetEditorPanel<FPropertyPanel>()->SetTarget(PickedComponent);
}

void FEditor::SetSceneClear()
{
	Gizmo->SetTarget(nullptr);
	Outline->SetTarget(nullptr);
	BoundingBox->SetTarget(nullptr);
	EditorUI->GetEditorPanel<FPropertyPanel>()->SetTarget(nullptr);
	ShowFlags.SetDefault();
	PickedComponent = nullptr;
}

void FEditor::Update(float DeltaTime, UCameraComponent* Camera, FMatrix VP, uint32 WinWidth, uint32 WinHeight)
{
	EditorUI->Tick(DeltaTime);

	FRay ray = Camera->DeProjection(FInputSystem::GetMouseX(), FInputSystem::GetMouseY(), WinWidth, WinHeight);
	FVector2 mousePos(FInputSystem::GetMouseX(), FInputSystem::GetMouseY());
	bool bMouseDown = FInputSystem::IsMouseDown(EMouseButton::Left);

	Gizmo->Update(ray, mousePos, VP, WinWidth, WinHeight, bMouseDown, Camera);

	// AActor* PickedActor = EditorUI->GetEditorPanel<FSceneOutlinerPanel>()->GetSelectedActor();
	
	

	if (FInputSystem::IsMousePressed(EMouseButton::Left) && !Gizmo->IsUsing() && Gizmo->GetHoveredAxis() < 0 && !ImGui::GetIO().WantCaptureMouse)
	{
		// UPrimitiveComponent* 
		AActor* Actor = Context.World->GetPickingPrimitive(WinWidth, WinHeight);
		if (!Actor)
		{
			PickedComponent = nullptr;
		}

		else
		{
			PickedComponent = Cast<UPrimitiveComponent>(Actor->GetRootComponent());
		}

		EditorUI->GetEditorPanel<FSceneOutlinerPanel>()->SetSelectedActor(Actor);
	}

	else if (PickedActor = EditorUI->GetEditorPanel<FSceneOutlinerPanel>()->GetSelectedActor())
	{
		PickedComponent = Cast<UPrimitiveComponent>(PickedActor->GetRootComponent());
	}
	SetTarget(PickedComponent);
	
}

void FEditor::OnRender(FMatrix VP, UCameraComponent* Camera, FRenderer* Renderer)
{
	const FVector CamLoc = Camera->GetLocation();

	if (Outline->GetTarget() && ShowFlags.IsSet(EShowFlagBits::OutLine) && ShowFlags.IsSet(EShowFlagBits::Primitives)
	&& Renderer->GetViewMode() != EViewModeIndex::Wireframe)
		OutlineRenderer->OnRender(*Outline, VP, CamLoc);

	UUIDBillboardRenderer->SetUUIDTextItemList(Camera);
	FontRenderer->RenderBatchTexts(UUIDBillboardRenderer->GetUUIDTextItemList(), Camera);
	Renderer->SetDepthStencilEnabled(true);



	if (ShowFlags.IsSet(EShowFlagBits::Grid))
	{
		DrawGrid(CamLoc);
	}

	// World 축 드로우
	AxisDraw();

	if (BoundingBox->GetTarget() && ShowFlags.IsSet(EShowFlagBits::BoundingBox))
	{
		LineRenderer->DrawBox(BoundingBox->GetTarget()->GetBounds());
	}

	// Line Rendering
	LineRenderer->Flush(Renderer, CamLoc, VP);

	ImGuiRenderer->Begin();

	EditorUI->OnRender();

	ImGuiRenderer->End();
}

// Gizmo Render 함수 분리
void FEditor::RenderGizmo(FMatrix VP, FRenderer* Renderer)
{
	if (Gizmo->GetTarget() && ShowFlags.IsSet(EShowFlagBits::Gizmo))
	{
		Renderer->SetDepthStencilEnabled(false);
		GizmoRenderer->OnRender(*Gizmo, VP);
		Renderer->SetDepthStencilEnabled(true);
	}
}

void FEditor::Shutdown()
{
	ImGuiRenderer->Shutdown();
}

void FEditor::DrawGrid(const FVector& CameraPos)
{
	// Grid 그리기
	float GridSpacing = EditorSettings->GetGridSpacing(); // EditorUI->GetEditorPanel<FControlPanel>()->GetGridSpace();
	GridCount = static_cast<int32>(GridExtent / GridSpacing);
	if (GridCount >= 100) GridCount = 100;

	const float CenterX = floor(CameraPos.X / GridSpacing) * GridSpacing;
	const float CenterY = floor(CameraPos.Y / GridSpacing) * GridSpacing;
	const float HalfSize = GridCount * GridSpacing;

	const FVector4 GridColor(1.0f, 1.0f, 1.0f, 1.0f);

	//for (int32 i = -GridCount; i <= GridCount; ++i)
	//{
	//	float Offset = i * GridSpacing;

	//	// X 방향 선
	//	LineRenderer->AddLine(
	//		FVector(CenterX - HalfSize, CenterY + Offset, 0.0f),
	//		FVector(CenterX + HalfSize, CenterY + Offset, 0.0f),
	//		GridColor);

	//	// Y 방향 선
	//	LineRenderer->AddLine(
	//		FVector(CenterX + Offset, CenterY - HalfSize, 0.0f),
	//		FVector(CenterX + Offset, CenterY + HalfSize, 0.0f),
	//		GridColor);
	//}

	const float AxisEpsilon = 0.001f;

	for (int32 i = -GridCount; i <= GridCount; ++i)
	{
		const float Offset = i * GridSpacing;

		// X 방향 그리드 선: Y=0이면 X축과 겹침
		const float GridY = CenterY + Offset;
		if (abs(GridY) > AxisEpsilon)
		{
			LineRenderer->AddLine(
				FVector(CenterX - HalfSize, GridY, 0.0f),
				FVector(CenterX + HalfSize, GridY, 0.0f),
				GridColor);
		}

		/*else
		{
			LineRenderer->AddLine(
				FVector(CenterX - HalfSize, GridY, 0.0f),
				FVector(CenterX + HalfSize, GridY, 0.0f),
				FVector4(1.0f, 0.0f, 0.0f, 1.0f));
		}*/

		// Y 방향 그리드 선: X=0이면 Y축과 겹침
		const float GridX = CenterX + Offset;
		if (abs(GridX) > AxisEpsilon)
		{
			LineRenderer->AddLine(
				FVector(GridX, CenterY - HalfSize, 0.0f),
				FVector(GridX, CenterY + HalfSize, 0.0f),
				GridColor);
		}

		/*else
		{
			LineRenderer->AddLine(
				FVector(GridX, CenterY - HalfSize, 0.0f),
				FVector(GridX, CenterY + HalfSize, 0.0f),
				FVector4(0.0f, 1.0f, 0.0f, 1.0f));
		}*/
	}

	// Z축 - 파랑
	LineRenderer->AddLine(
		FVector(0.0f, 0.0f, -1000.0f),
		FVector(0.0f, 0.0f, 1000.0f),
		FVector4(0.0f, 0.0f, 1.0f, 1.0f)
	);
}

void FEditor::ClearSceneTargetsAndFlags()
{
	PickedComponent = nullptr;
	PickedActor = nullptr;
	EditorUI->GetEditorPanel<FSceneOutlinerPanel>()->SetSelectedActor(nullptr);
	Gizmo->SetTarget(nullptr);
	Outline->SetTarget(nullptr);
	BoundingBox->SetTarget(nullptr);
	EditorUI->GetEditorPanel<FPropertyPanel>()->SetTarget(nullptr);
	ShowFlags.SetDefault();
}

void FEditor::AxisDraw()
{
	// X축 - 빨강
	LineRenderer->AddLine(
		FVector(-100000.0f, 0.0f, 0.0f),
		FVector(100000.0f, 0.0f, 0.0f),
		FVector4(1.0f, 0.0f, 0.0f, 1.0f)
	);

	// Y축 - 초록
	LineRenderer->AddLine(
		FVector(0.0f, -100000.0f, 0.0f),
		FVector(0.0f, 100000.0f, 0.0f),
		FVector4(0.0f, 1.0f, 0.0f, 1.0f)
	);

	// Z축 - 파랑
	LineRenderer->AddLine(
		FVector(0.0f, 0.0f, -10000.0f),
		FVector(0.0f, 0.0f, 10000.0f),
		FVector4(0.0f, 0.0f, 1.0f, 1.0f)
	);

}