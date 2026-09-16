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

                //if (ImGui::Selectable(ActorName.c_str(), Actor == SelectedActor))
                //{
                //    SelectedActor = (SelectedActor == Actor) ? nullptr : Actor;
                //    // SelectedActor = Actor;
                //}

                ImGuiTreeNodeFlags ActorFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

                // 자식 Component가 없으면 Leaf
                if (Actor->GetComponents().empty())
                {
                    ActorFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                }

                bool bActorOpen = ImGui::TreeNodeEx(ActorName.c_str(), ActorFlags);

                // Actor 선택
                if (ImGui::IsItemClicked())
                {
                    SelectedActor = (SelectedActor == Actor) ? nullptr : Actor;
                }

                // Component 목록
                if (bActorOpen)
                {
                    for (UActorComponent* Component : Actor->GetComponents())
                    {
                        if (!Component)
                        {
                            continue;
                        }

                        ImGui::PushID(Component);

                        FString ComponentName = Component->GetFName().GetString();

                        ImGui::TreeNodeEx(
                            ComponentName.c_str(),
                            ImGuiTreeNodeFlags_Leaf |
                            ImGuiTreeNodeFlags_NoTreePushOnOpen |
                            ImGuiTreeNodeFlags_SpanAvailWidth
                        );

                        ImGui::PopID();
                    }

                    ImGui::TreePop();
                }

                ImGui::PopID();
            }

            ImGui::TreePop();
        }
    }

	ImGui::End();
}