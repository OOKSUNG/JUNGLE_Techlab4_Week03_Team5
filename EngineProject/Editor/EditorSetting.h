#pragma once
#include <filesystem>
#include <windows.h>
#include <string>

class FEditorSettings
{
	/*static FEditorSettings& Get()
	{
		static FEditorSettings Instance;
		return Instance;
	}*/
public:
	void LoadEditorSetting(const std::string& FilePath = "editor.ini");
	void SaveEditorSetting(const std::string& FilePath = "editor.ini");

	void SetCameraMoveSpeed(float Speed) { CameraMoveSpeed = Speed; }
	float GetCameraMoveSpeed() { return CameraMoveSpeed; }

	void SetCameraSensitivity(float Sensitivity) { CameraSensitivity = Sensitivity; }
	float GetCameraSensitivity() { return CameraSensitivity; }

	void SetGridSpacing(float Space) { GridSpacing = Space; }
	float GetGridSpacing() { return GridSpacing; }

private:
	float CameraMoveSpeed = 10.0f;
	float CameraSensitivity = 0.05f;
	int32 GridSpacing = 10;
};
