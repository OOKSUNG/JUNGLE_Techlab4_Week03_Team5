#pragma once
#include <filesystem>
#include <windows.h>
#include <string>

struct FEditorSettings
{
	static FEditorSettings& Get()
	{
		static FEditorSettings Instance;
		return Instance;
	}

	void LoadEditorSetting(const std::string& FilePath = "editor.ini");
	void SaveEditorSetting(const std::string& FilePath = "editor.ini");

	float CameraMoveSpeed = 10.0f;
	float CameraSensitivity = 0.05f;
	int32 GridSpacing = 10;
};
