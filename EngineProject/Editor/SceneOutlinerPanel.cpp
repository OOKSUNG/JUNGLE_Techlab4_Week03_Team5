#include "EnginePCH.h"
#include "SceneOutlinerPanel.h"

bool FSceneOutlinerPanel::Init()
{
	return true;
}

void FSceneOutlinerPanel::Tick(float DeltaTime)
{

}

void FSceneOutlinerPanel::OnRender()
{
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	ImGui::Begin("Outliner");
    for (AActor* Actor : Context.World->GetActors())
    {
        if (!Actor)
        {
            continue;
        }

        FString ActorName = Actor->GetFName().GetString();

        // 아웃라이너 항목 그리기
        ImGui::Text("%s", ActorName.c_str());
    }
	ImGui::End();
}