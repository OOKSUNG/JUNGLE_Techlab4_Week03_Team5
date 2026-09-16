#include "EnginePCH.h"
#include "PropertyPanel.h"
#include "Component/TextComponent.h"
#include "Component/ParticleSubUVComponent.h"
#include "Text/FontManager.h"

#include "imgui_internal.h"

namespace
{
	bool DrawAxisControl(const FString& _label, float& _value, float _speed, float _minValue, float _maxValue, float _resetValue, FVector4 _color)
	{
		bool isValueChanged = false;

		float lineHeight = GImGui->Font->LegacySize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec4 color(_color.X, _color.Y, _color.Z, _color.W);
		ImVec2 buttonSize = { lineHeight + 2.0f , lineHeight };
		// 각 버튼의 색상 설정
		ImGui::PushStyleColor(ImGuiCol_Button, color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ color.x + 0.1f, color.y + 0.1f, color.z + 0.1f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ color.x - 0.1f, color.y - 0.1f, color.z - 0.1f, 1.0f });

		// 굵은 폰트 적용
		if (ImGui::Button(_label.c_str(), buttonSize))
		{
			_value = _resetValue;
			isValueChanged = true;
		}
		ImGui::PopStyleColor(3);

		// 드래그 슬라이더
		ImGui::SameLine();
		std::string dragID = "##" + _label;
		isValueChanged |= ImGui::DragFloat(dragID.c_str(), &_value, _speed, _minValue, _maxValue, "%.2f");

		return isValueChanged;

	}

	bool DrawVector3Controller(const FString& _label, FVector& _values, float _resetValue, float _columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGuiTableFlags flags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable;

		bool isVectorChanged = false;

		ImGui::PushID(_label.c_str());
		if (ImGui::BeginTable(_label.c_str(), 2, flags)) // 고유 ID, 열 2개, 플래그
		{
			// ImGuiTableColumnFlags_WidthFixed: 초기 너비 고정
			// ImGuiTableColumnFlags_WidthStretch: 창 크기에 따라 너비 조절 (기본값)
			ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, _columnWidth);
			ImGui::TableSetupColumn("Content", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text(_label.c_str()); // 왼쪽 열: 레이블

			ImGui::TableSetColumnIndex(1);
			// [컨트롤러 UI 코드] // 오른쪽 열: 컨트롤러
			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 2,0 });
			ImGui::PushFont(boldFont);

			isVectorChanged |= DrawAxisControl("X", _values.X, 0.1f, 0.0f, 0.0f, 0.0f, { 0.8f, 0.1f,0.1f, 1.0f });

			ImGui::PopItemWidth();

			ImGui::SameLine();
			isVectorChanged |= DrawAxisControl("Y", _values.Y, 0.1f, 0.0f, 0.0f, 0.0f, { 0.1f, 0.8f,0.1f, 1.0f });

			ImGui::PopItemWidth();

			ImGui::SameLine();
			isVectorChanged |= DrawAxisControl("Z", _values.Z, 0.1f, 0.0f, 0.0f, 0.0f, { 0.1f, 0.1f,0.8f, 1.0f });

			ImGui::PopItemWidth();

			ImGui::PopFont();
			ImGui::PopStyleVar();
			ImGui::EndTable();
		}

		ImGui::Columns(1);

		ImGui::PopID();

		return isVectorChanged;
	}
}

bool FPropertyPanel::Init()
{
	//transform = new FTransform(FVector(), FVector(), FRotator());

	return true;
}

void FPropertyPanel::Tick(float DeltaTime)
{

}


void FPropertyPanel::OnRender()
{
	ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);

	ImGui::Begin("Jungle Property Window");

	FTransform* transform = nullptr;

	if (Target)
	{
		transform = Target->GetTransform();

		DrawVector3Controller("Translation", transform->Location, 0.0f, 70.0f);
		FVector Rotation = FVector(transform->Rotation.Roll, transform->Rotation.Pitch, transform->Rotation.Yaw);
		DrawVector3Controller("Rotation", Rotation, 0.0f, 70.0f);
		transform->Rotation = FRotator(Rotation.Y, Rotation.Z, Rotation.X);
		DrawVector3Controller("Scale", transform->Scale, 0.0f, 70.0f);
		if (UPrimitiveComponent* TargetPrimitive = Cast<UPrimitiveComponent>(Target))
		{
			bool bIsVisible = TargetPrimitive->GetVisible();
			if (ImGui::Checkbox("IsVisible", &bIsVisible))
			{
				TargetPrimitive->SetVisible(bIsVisible);
			}
		}
		if (UParticleSubUVComponent* TargetParticle = Cast<UParticleSubUVComponent>(Target))
		{
			bool bLoop = TargetParticle->GetLoop();
			if (ImGui::Checkbox("Loop", &bLoop))
			{
				TargetParticle->SetLoop(bLoop);
			}
			float PlayRate = TargetParticle->GetPlayRate();
			if (ImGui::DragFloat("PlayRate", &PlayRate, 0.05f, 0.1f, 5.0f, "%.2f"))
			{
				TargetParticle->SetPlayRate(PlayRate);
			}
		}

		if (UTextComponent* TargetText = Cast<UTextComponent>(Target))
		{
			// Font Input 입력
			char TextBuffer[256];
			strncpy_s(TextBuffer, TargetText->GetText().c_str(), sizeof(TextBuffer)-1);
			if (ImGui::InputText("Text", TextBuffer, sizeof(TextBuffer)))
			{
				TargetText->SetText(TextBuffer);
			}

			// Font Color 선택
			FVector4 Color = TargetText->GetColor();
			float ColorArr[4] = { Color.X, Color.Y, Color.Z, Color.W };
			if(ImGui::ColorEdit4("Color", ColorArr))
			{
				TargetText->SetColor(FVector4(ColorArr[0],ColorArr[1],ColorArr[2],ColorArr[3]));
			}

			// FontPath 선택
			const TArray<FString>& FontPaths = FFontManager::GetInstance().GetAvailableFontPaths();
			FString CurrentFontPath = TargetText->GetFontPath();

			// Font 이름만 Parsing
			auto GetDisplayName = [](const FString& Path) -> FString
			{
				size_t Slash = Path.find_last_of("/\\");
				return (Slash == FString::npos) ? Path : Path.substr(Slash + 1);
			};

			if (ImGui::BeginCombo("Font", GetDisplayName(CurrentFontPath).c_str()))
			{
				for (const FString& Path : FontPaths)
				{
					bool bSelected = (Path == CurrentFontPath);
					if (ImGui::Selectable(GetDisplayName(Path).c_str(), bSelected))
					{
						TargetText->SetFontPath(Path);
					}
					if (bSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			// FontSize 입력
			int FontSize = TargetText->GetFontPixelSize();
			if (ImGui::SliderInt("Font Size", &FontSize, 1, 100))
			{
				TargetText->SetFontPixelSize(FontSize);
			}
		}
	}

	ImGui::End();
}

FPropertyPanel::~FPropertyPanel()
{
	//delete transform;
}