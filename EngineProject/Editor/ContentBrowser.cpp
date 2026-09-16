#include "EnginePCH.h"
#include "ContentBrowser.h"

bool Init()
{


    return true;
}

void Tick(float DeltaTime)
{

}

void OnRender()
{
    //fs::path currentDirectory = "";
    //int columnCount = 5;
    //ImGui::Columns(columnCount, nullptr, false);

    //for (const auto& entry : fs::directory_iterator(currentDirectory))
    //{
    //    auto path = entry.path();
    //    auto name = path.filename().string();

    //    // 여기에는 실제 텍스처 아이콘을 ImageButton으로 표시할 수도 있음
    //    ImGui::Button(entry.is_directory() ? "[DIR]" : "[FILE]",
    //        ImVec2(72, 72));

    //    if (ImGui::IsItemHovered() &&
    //        ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) &&
    //        entry.is_directory())
    //    {
    //        currentDirectory = path;
    //    }

    //    ImGui::TextWrapped("%s", name.c_str());
    //    ImGui::NextColumn();
    //}

    //ImGui::Columns(1);
}




