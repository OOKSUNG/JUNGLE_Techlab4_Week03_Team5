#include "EnginePCH.h"
#include "EditorUI.h"

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


	GetEditorPanel<FConsolePanel>()->AddLog(ELogVerbosity::Info, "Engine Initialize...");

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
