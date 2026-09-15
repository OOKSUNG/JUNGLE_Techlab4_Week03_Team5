#include "EnginePCH.h"
#include "EditorUI.h"
#include "ConsolePanel.h"
#include "ControlPanel.h"
#include "PropertyPanel.h"


bool FEditorUI::Init(const FEditorContext& InContext)
{
	// 컨텍스트 초기화
	Context = InContext;

	// 패널 추가
	AddEditorPanel<FConsolePanel>();
	AddEditorPanel<FPropertyPanel>();
	AddEditorPanel<FControlPanel>();

	GetEditorPanel<FPropertyPanel>()->FPropertyPanel::SetContext(InContext);
	GetEditorPanel<FControlPanel>()->FControlPanel::SetContext(InContext);
	GetEditorPanel<FConsolePanel>()->FConsolePanel::SetContext(InContext);


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
	for (auto& Panel : Panels)
	{
		Panel->OnRender();
	}
}

