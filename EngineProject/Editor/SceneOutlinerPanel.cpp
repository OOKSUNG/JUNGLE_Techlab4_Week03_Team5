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
        ImGuiTreeNodeFlags SceneFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

        if (ImGui::TreeNodeEx("Scene", SceneFlags))
        {
            //for (AActor* Actor : Context.World->GetActors())
            //{
            //    if (!Actor)
            //    {
            //        continue;
            //    }

            //    ImGui::PushID(Actor->GetUUID());

            //    FString ActorName = Actor->GetFName().GetString();

            //    ImGuiTreeNodeFlags ActorFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

            //    // 자식 Component가 없으면 Leaf
            //    if (Actor->GetComponents().empty())
            //    {
            //        ActorFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            //    }

            //    bool bActorOpen = ImGui::TreeNodeEx(ActorName.c_str(), ActorFlags);

            //    // Actor 선택
            //    if (ImGui::IsItemClicked())
            //    {
            //        SelectedActor = (SelectedActor == Actor) ? nullptr : Actor;
            //    }

            //    // Component 목록
            //    if (bActorOpen)
            //    {
            //        for (UActorComponent* Component : Actor->GetComponents())
            //        {
            //            if (!Component)
            //            {
            //                continue;
            //            }

            //            ImGui::PushID(Component);

            //            FString ComponentName = Component->GetFName().GetString();

            //            ImGui::TreeNodeEx(
            //                ComponentName.c_str(),
            //                ImGuiTreeNodeFlags_Leaf |
            //                ImGuiTreeNodeFlags_NoTreePushOnOpen |
            //                ImGuiTreeNodeFlags_SpanAvailWidth
            //            );

            //            ImGui::PopID();
            //        }

            //        ImGui::TreePop();
            //    }

            //    ImGui::PopID();
            //}

            for (AActor* Actor : Context.World->GetActors())
            {
                if (!Actor) continue;

                ImGui::PushID(Actor->GetUUID());

                FString ActorName = Actor->GetFName().GetString();

                // 현재 이름 편집 중인 Actor
                if (RenameTarget == Actor)
                {
                    ImGui::SetKeyboardFocusHere();

                    if (ImGui::InputText( "##Rename", RenameBuffer, IM_ARRAYSIZE(RenameBuffer),
                        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
                    {
                        if (RenameBuffer[0] != '\0')
                        {
                            Actor->SetFName(FString(RenameBuffer));
                        }

                        RenameTarget = nullptr;
                    }

                    // ESC로 취소
                    if (ImGui::IsKeyPressed(ImGuiKey_Escape))
                    {
                        RenameTarget = nullptr;
                    }
                }
                else
                {
                    if (ImGui::Selectable( ActorName.c_str(), Actor == SelectedActor))
                    {
                        SelectedActor = (SelectedActor == Actor) ? nullptr : Actor;
                    }

                    // 더블 클릭하면 이름 변경
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    {
                        RenameTarget = Actor;
                        FString CurrentName = Actor->GetFName().GetString();
                        strcpy_s(RenameBuffer, sizeof(RenameBuffer), CurrentName.c_str());
                    }
                }

                ImGui::PopID();
            }

            ImGui::TreePop();
        }
    }

	ImGui::End();
}