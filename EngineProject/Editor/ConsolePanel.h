#pragma once

#include "EditorPanel.h"
#include "../Core/EngineLog.h"

class FConsolePanel : public IEditorPanel
{
public:

	bool Init() override;
	void Tick(float DeltaTime) override;
	void OnRender() override;
	void ClearLog();
	void ExecCommand(const FString& CommandLine);
	int TextEditCallback(ImGuiInputTextCallbackData* Data);

private:
	ImVec4 GetLogColor(ELogVerbosity Verbosity) const;
	const char* GetVerbosityName(ELogVerbosity Verbosity) const;
	const char* GetCategoryName(ELogCategory Category) const;

private:

	char InputBuf[256]{};

	TArray<FString> Commands;
	TArray<FString> History;

	int HistoryPos = -1;

	ImGuiTextFilter Filter;

	bool AutoScroll = true;
	bool ScrollToBottom = false;
};











// 기존 코드

//#pragma once
//
//#include <format>
//#include "EditorPanel.h"
//
//enum class ELogVerbosity
//{
//	Input,
//	Info,
//	Warning,
//	Error
//};
//
//struct FLogData
//{
//	FString message;
//	ImVec4 Color;
//};
//
//class FConsolePanel : public IEditorPanel
//{
//public:
//	bool Init() override;
//	void Tick(float DeltaTime)override;
//	void OnRender() override;
//
//	void ClearLog();
//
//	template<typename... Args>
//	void AddLog(ELogVerbosity Verbosity, std::format_string<Args...> fmt, Args&&... args)
//	{
//		FLogData LogData;
//
//
//		// 여기서 Category / Verbosity 활용
//		switch (Verbosity)
//		{
//		case ELogVerbosity::Input:
//			LogData.Color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
//			break;
//		case ELogVerbosity::Info:
//			LogData.Color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
//			break;
//		case ELogVerbosity::Warning:
//			LogData.Color = ImVec4(1.0f, 0.75f, 0.2f, 1.0f);
//			break;
//		case ELogVerbosity::Error:
//			LogData.Color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
//			break;
//		}
//
//		LogData.message = std::format(fmt, std::forward<Args>(args)...);
//
//		Items.push_back(LogData);
//
//		ScrollToBottom = true;  
//	}
//
//	void ExecCommand(const FString& command_line);
//
//	int TextEditCallback(ImGuiInputTextCallbackData* data);
//
//private:
//	char                  InputBuf[256];
//	TArray<FLogData>       Items;
//	TArray<FString>		  Commands;
//	TArray<FString>       History;
//	int                   HistoryPos;
//	ImGuiTextFilter       Filter;
//	bool                  AutoScroll;
//	bool                  ScrollToBottom;
//};