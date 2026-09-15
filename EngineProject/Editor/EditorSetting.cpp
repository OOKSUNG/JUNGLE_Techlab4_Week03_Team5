#include "EnginePCH.h" 
#include "EditorSetting.h"

void FEditorSettings::LoadEditorSetting(const std::string& FilePath)
{
	std::filesystem::path BaseDir = L".\\Editor";
	std::filesystem::path FullPath = BaseDir / FilePath;
	const wchar_t* IniPath = FullPath.c_str();

	wchar_t Buffer[64];
	GetPrivateProfileStringW(L"Grid", L"GridSpacing", L"10.0", Buffer, 64, IniPath);
	GridSpacing = std::stoi(Buffer);
	GetPrivateProfileStringW(L"Editor", L"CameraSpeed", L"10.0", Buffer, 64, IniPath);
	CameraMoveSpeed = std::stof(Buffer);
	GetPrivateProfileStringW(L"Editor", L"CameraSensitivity", L"0.05", Buffer, 64, IniPath);
	CameraSensitivity = std::stof(Buffer);
}

void FEditorSettings::SaveEditorSetting(const std::string& FilePath)
{
	std::filesystem::path BaseDir = L".\\Editor";
	std::filesystem::path FullPath = BaseDir / FilePath;
	const wchar_t* IniPath = FullPath.c_str();

	WritePrivateProfileStringW(L"Grid", L"GridSpacing", std::to_wstring(GridSpacing).c_str(), IniPath);
	WritePrivateProfileStringW(L"Editor", L"CameraSpeed", std::to_wstring(CameraMoveSpeed).c_str(), IniPath);
	WritePrivateProfileStringW(L"Editor", L"CameraSensitivity", std::to_wstring(CameraSensitivity).c_str(), IniPath);
}