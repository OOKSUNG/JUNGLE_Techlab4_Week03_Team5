#include "EnginePCH.h"
#include "SceneOutlinerPanel.h"

bool FSceneOutlinerPanel::Init( )
{


	return true;
}

void FSceneOutlinerPanel::Tick(float DeltaTime)
{
    
}

void FSceneOutlinerPanel::OnRender()
{
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Scene Outliner"))
    {
        ImGuiTreeNodeFlags SceneFlags =
            ImGuiTreeNodeFlags_DefaultOpen |
            ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_SpanAvailWidth;

        if (ImGui::TreeNodeEx("Scene", SceneFlags))
        {
            for (AActor* Actor : Context.World->GetActors())
            {
                if (!Actor)
                {
                    continue;
                }

                ImGui::PushID(Actor->GetUUID());

                FString ActorName = Actor->GetFName().GetString();

                /*ImGui::TreeNodeEx(
                    ActorName.c_str(),
                    ImGuiTreeNodeFlags_Leaf |
                    ImGuiTreeNodeFlags_NoTreePushOnOpen |
                    ImGuiTreeNodeFlags_SpanAvailWidth
                );*/

                if (ImGui::Selectable(ActorName.c_str(), Actor == SelectedActor))
                {
                    SelectedActor = (SelectedActor == Actor) ? nullptr : Actor;
                    // SelectedActor = Actor;
                }

                ImGui::PopID();
            }

            ImGui::TreePop();
        }
    }

	ImGui::End();
}