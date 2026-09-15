#include "EnginePCH.h"
#include "ControlPanel.h"
#include "../Camera/CameraActor.h"
#include "../Camera/CameraComponent.h"
#include "../Core/EngineTimer.h"
#include "../Editor/Gizmo.h"
#include "Input/InputSystem.h"

bool FControlPanel::Init()
{



	return true;
}

void FControlPanel::Tick(float DeltaTime)
{
	if (FInputSystem::IsKeyPressed(EKeyCode::Space))
	{
		static int ModeIndex = 0;
		ModeIndex = (ModeIndex + 1) % 3;
		Context.Gizmo->SetMode(static_cast<EGizmoMode>(ModeIndex));
	}

	FControlPanel::DeltaTime = DeltaTime;
}

void FControlPanel::AddActor(EPrimitiveType Type)
{
	FTransform Transform;
	AActor* Actor = Context.World->SpawnActor(AActor::StaticClass(), &Transform);
	Actor->AddPrimitiveComponent(Type, Transform);


	ActorNum = Context.World->GetActorNum() - 1;
}


void FControlPanel::OnRender()
{
	ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
	ImGui::Begin("Jungle Control Panel");

	ImGui::Text("Hello Jungle World");

	// FPS 출력
	ImGui::Text("FPS: %.2f (%.0f ms)", 1.0f / DeltaTime, DeltaTime * 1000.0f);

	ImGui::Separator();

	// Primitive Spawn
	ImGui::SetNextItemWidth(130.0f);
	ImGui::Combo("Actor", &SelectedIndex, Items, IM_ARRAYSIZE(Items));
	ImGui::SameLine();
	ImGui::Text("Primitive");
	//EngineTimer::GetDeltaTime();
	if (ImGui::SmallButton("Spawn")) { AddActor(static_cast<EPrimitiveType>(SelectedIndex)); }
	ImGui::SameLine();
	ImGui::SetNextItemWidth(80.0f);
	ImGui::InputInt("##N", &ActorNum, 0, 0, ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine();
	ImGui::Text("Number of spawn");

	ImGui::Separator();

	// 씬 생성 세이브 로드
	ImGui::SetNextItemWidth(165.0f);
	if (ImGui::Button("New Scene", ImVec2(80.0f, 19.0f))) 
	{
		if (NewSceneCallback) NewSceneCallback();
		ActorNum = Context.World->GetActorNum() - 1;
	}
	if (ImGui::Button("Save Scene", ImVec2(80.0f, 19.0f))) 
	{ 
		if (SaveSceneCallback) SaveSceneCallback();
		ActorNum = Context.World->GetActorNum() - 1; 
	}
	if (ImGui::Button("Load Scene", ImVec2(80.0f, 19.0f)))
	{
		if (LoadSceneCallback) LoadSceneCallback();
		ActorNum = Context.World->GetActorNum() - 1;
	}

	ImGui::Separator();
	
	// Camera Editor
	UCameraComponent* CamCom = Context.World->GetMainCamera()->GetCameraComponent();
	
	ImGui::Checkbox("Orthogonal", &CamCom->bIsOrthogonal);

	FTransform* transform = CamCom->GetTransform();
	float MouseSensitivity = CamCom->GetSensitivity();
	float CameraSpeed = CamCom->GetSpeed();

	ImGui::SetNextItemWidth(255.0f);
	ImGui::InputFloat("##FOV", &CamCom->FOV);

	ImGui::SetNextItemWidth(80.0f);
	ImGui::InputFloat("##X", &transform->Location.X);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(80.0f);
	ImGui::InputFloat("##Y", &transform->Location.Y);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(80.0f);
	ImGui::InputFloat("##Z", &transform->Location.Z);
	ImGui::SameLine();
	ImGui::Text("Camera Location");

	ImGui::SetNextItemWidth(80.0f);
	ImGui::InputFloat("##Pitch", &transform->Rotation.Pitch);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(80.0f);
	ImGui::InputFloat("##Yaw", &transform->Rotation.Yaw);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(80.0f);
	ImGui::InputFloat("##Roll", &transform->Rotation.Roll);
	ImGui::SameLine();
	ImGui::Text("Camera Rotation");
	if (ImGui::SliderFloat("Camera Sensitivity", &MouseSensitivity, 0.01f, 0.5f, "%.2f"))
	{
		CamCom->SetSensitivity(MouseSensitivity);
		FEditorSettings::Get().CameraSensitivity = MouseSensitivity;
	}
	if (ImGui::SliderFloat("Camera Speed", &CameraSpeed, 1.0f, 50.0f))
	{
		CamCom->SetSpeed(CameraSpeed);
		FEditorSettings::Get().CameraMoveSpeed = CameraSpeed;
	}

	ImGui::Separator();

	if(ImGui::SliderFloat("Grid Size", &GridSpace, 1.0f, 100.0f))
	{
		FEditorSettings::Get().GridSpacing = GridSpace;
	}

	ImGui::Separator();

	// Gizmo Select
	GizmoSelectedIndex = static_cast<int32>(Context.Gizmo->GetMode());
	if (ImGui::SetNextItemWidth(100.0f); ImGui::Combo("##GizmoCombo", &GizmoSelectedIndex, GizmoItems, IM_ARRAYSIZE(GizmoItems)))
	{
		Context.Gizmo->SetMode(static_cast<EGizmoMode>(GizmoSelectedIndex));
	}

	if (ImGui::SetNextItemWidth(100.0f); ImGui::Combo("##SpaceCombo", &SpaceSelectedIndex, SpaceItems, IM_ARRAYSIZE(SpaceItems)))
	{
		Context.Gizmo->SetSpace(static_cast<EGizmoSpace>(SpaceSelectedIndex));
	}

	ImGui::Text("Show Flags");
	bool bGrid = ShowFlags->IsSet(EShowFlagBits::Grid);
	if (ImGui::Checkbox("World Grid", &bGrid))
	{
		ShowFlags->Set(EShowFlagBits::Grid, bGrid);
	}
	ImGui::SameLine();
	bool bPrimitives = ShowFlags->IsSet(EShowFlagBits::Primitives);
	if (ImGui::Checkbox("Primitives", &bPrimitives))
	{
		ShowFlags->Set(EShowFlagBits::Primitives, bPrimitives);
	}
	ImGui::SameLine();
	bool bOutLine = ShowFlags->IsSet(EShowFlagBits::OutLine);
	if (ImGui::Checkbox("OutLine", &bOutLine))
	{
		ShowFlags->Set(EShowFlagBits::OutLine, bOutLine);
	}
	ImGui::SameLine();
	bool bGizmo = ShowFlags->IsSet(EShowFlagBits::Gizmo);
	if (ImGui::Checkbox("Gizmo", &bGizmo))
	{
		ShowFlags->Set(EShowFlagBits::Gizmo, bGizmo);
	}
	ImGui::SameLine();
	bool bBoundingBox = ShowFlags->IsSet(EShowFlagBits::BoundingBox);
	if (ImGui::Checkbox("Bounding Box", &bBoundingBox))
	{
		ShowFlags->Set(EShowFlagBits::BoundingBox, bBoundingBox);
	}
	ImGui::Text("ShowFlagPreset");
	if (ImGui::Button("Default"))
	{
		ShowFlags->SetFlagPreset(EShowFlagBits::Default);
	}
	ImGui::SameLine();
	if (ImGui::Button("None"))
	{
		ShowFlags->SetFlagPreset(EShowFlagBits::None);
	}

	// View Mode Select
	ViewModeIndex = static_cast<int32>(Renderer->GetViewMode());
	if (ImGui::RadioButton("Lit", &ViewModeIndex, 0)) Renderer->SetViewMode(EViewModeIndex::Lit);
	ImGui::SameLine();
	if (ImGui::RadioButton("Unlit", &ViewModeIndex, 1)) Renderer->SetViewMode(EViewModeIndex::Unlit);
	ImGui::SameLine();
	if (ImGui::RadioButton("Wirframe", &ViewModeIndex, 2)) Renderer->SetViewMode(EViewModeIndex::Wireframe);
	
	ImGui::End();
}
