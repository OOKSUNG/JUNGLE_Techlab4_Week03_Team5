#include "EnginePCH.h"
#include "EditorUI.h"
#include "ConsolePanel.h"
#include "ControlPanel.h"
#include "PropertyPanel.h"
#include "SceneOutlinerPanel.h"
#include "imgui_internal.h"



bool FEditorUI::Init(const FEditorContext& InContext)
{
	// 컨텍스트 초기화
	Context = InContext;

	// 패널 추가
	AddEditorPanel<FConsolePanel>();
	GetEditorPanel<FConsolePanel>()->FConsolePanel::SetContext(InContext);

	AddEditorPanel<FPropertyPanel>();
	AddEditorPanel<FControlPanel>();
	AddEditorPanel<FSceneOutlinerPanel>();

	/*GetEditorPanel<FPropertyPanel>()->FPropertyPanel::SetContext(InContext);
	GetEditorPanel<FControlPanel>()->FControlPanel::SetContext(InContext);
	GetEditorPanel<FSceneOutlinerPanel>()->FSceneOutlinerPanel::SetContext(InContext);*/
	

	return true;
}

void FEditorUI::Tick(float DeltaTime)
{
	for (auto& Panel : Panels)
	{
		Panel->Tick(DeltaTime);
	}
}

void FEditorUI::OnRender()
{
	
	ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");

	ImGui::DockSpaceOverViewport(dockspace_id, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

	if (!ImGui::DockBuilderGetNode(dockspace_id)) // 최초 1회만
	{
		ImGui::DockBuilderRemoveNode(dockspace_id);
		ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

		ImGuiID dock_left, dock_right, dock_bottom, dock_main = dockspace_id;
		ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left, 0.2f, &dock_left, &dock_main);
		ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.25f, &dock_right, &dock_main);
		ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Down, 0.25f, &dock_bottom, &dock_main);

		ImGui::DockBuilderDockWindow("Jungle Control Panel", dock_left);
		ImGui::DockBuilderDockWindow("Scene Outliner", dock_right);
		ImGui::DockBuilderDockWindow("Jungle Property Window", dock_right);
		ImGui::DockBuilderDockWindow("Console", dock_bottom);

		ImGui::DockBuilderFinish(dockspace_id);
	}
	for (auto& Panel : Panels)
	{
		Panel->OnRender();
	}
}

