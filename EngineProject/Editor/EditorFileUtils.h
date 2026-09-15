#pragma once

#include "Engine/SceneMetaData.h"
#include <windows.h>
#include <shobjidl.h>

class FEditorFileUtils
{
public:
	bool LoadSceneFromFileSelection(FSceneMetaData& SceneData);
	bool SaveSceneWithFileBrowser(FSceneMetaData& SceneData);
private:
	void ReleaseFileOpenResource
	(
		IFileOpenDialog* FileOpen = nullptr,
		IShellItem* Items = nullptr,
		PWSTR PWPath = nullptr
	);
	void ReleaseFileSaveResource
	(
		IFileSaveDialog* FileSave = nullptr,
		IShellItem* Items = nullptr,
		PWSTR PWPath = nullptr
	);
};