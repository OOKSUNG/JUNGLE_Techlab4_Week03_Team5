#include "EditorFileUtils.h"
#include "Core/EngineString.h"
#include <fstream>
#include "json.hpp"
#include <iostream>
#include <format>

using json = nlohmann::ordered_json;

bool FEditorFileUtils::LoadSceneFromFileSelection(FSceneMetaData& SceneData)
{
	HRESULT Hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(Hr))
	{
		return false;
	}

	IFileOpenDialog* FileSystem;
	Hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&FileSystem));
	if (FAILED(Hr)) 
	{
		CoUninitialize();
		return false;
	}

	COMDLG_FILTERSPEC Filter[1];
	Filter[0].pszName = L"Scene Files (*.Scene)";
	Filter[0].pszSpec = L"*.Scene";
	Hr = FileSystem->SetFileTypes(1, Filter);
	if (FAILED(Hr))
	{
		ReleaseFileOpenResource(FileSystem);
		return false;
	}
	Hr = FileSystem->SetFileTypeIndex(1);
	if (FAILED(Hr))
	{
		ReleaseFileOpenResource(FileSystem);
		return false;
	}

	FILEOPENDIALOGOPTIONS options;
	Hr = FileSystem->GetOptions(&options);
	if (FAILED(Hr))
	{
		ReleaseFileOpenResource(FileSystem);
		return false;
	}

	Hr = FileSystem->SetOptions(options | FOS_STRICTFILETYPES);
	if (FAILED(Hr))
	{
		ReleaseFileOpenResource(FileSystem);
		return false;
	}

	//  SHOW OPEN FILE DIALOG WINDOW
	Hr = FileSystem->Show(NULL);
	if (FAILED(Hr)) 
	{
		ReleaseFileOpenResource(FileSystem);
		return false;
	}

	//  RETRIEVE FILE NAME FROM THE SELECTED ITEM
	IShellItem* Files;
	Hr = FileSystem->GetResult(&Files);
	if (FAILED(Hr)) 
	{
		ReleaseFileOpenResource(FileSystem);
		return false;
	}

	//  STORE AND CONVERT THE FILE NAME
	PWSTR PWPath = nullptr;
	Hr = Files->GetDisplayName(SIGDN_FILESYSPATH, &PWPath);
	if (FAILED(Hr)) 
	{
		ReleaseFileOpenResource(FileSystem, Files);
		return false;
	}

	std::filesystem::path FSPath(PWPath);
	FString Path = FSPath.string();
	std::ifstream File(Path);

	if (!File.is_open())
	{
		ReleaseFileOpenResource(FileSystem, Files, PWPath);
		return false;
	}

	json Json;

	try
	{
		File >> Json;
	}
	catch (const json::parse_error&)
	{
		File.close();
		ReleaseFileOpenResource(FileSystem, Files, PWPath);
		return false;
	}

	File.close();

	if (!Json.contains("Version"))
	{
		ReleaseFileOpenResource(FileSystem, Files, PWPath);
		return false;
	}
	
	if (Json["Version"] != 1)
	{
		ReleaseFileOpenResource(FileSystem, Files, PWPath);
		return false;
	}

	SceneData.Version = 1;

	if (Json.contains("NextUUID"))
	{
		SceneData.NextUUID = Json["NextUUID"].get<uint32>();
	}

	if (!Json.contains("Primitives"))
	{
		ReleaseFileOpenResource(FileSystem, Files, PWPath);
		return true;
	}

	for (auto& [UUIDString, PrimitiveJson] : Json["Primitives"].items())
	{
		uint32 UUID = std::stoul(UUIDString);

		FTransform Transform;

		if (PrimitiveJson.contains("Location"))
		{
			Transform.Location.X = PrimitiveJson["Location"][0].get<float>();
			Transform.Location.Y = PrimitiveJson["Location"][1].get<float>();
			Transform.Location.Z = PrimitiveJson["Location"][2].get<float>();
		}

		if (PrimitiveJson.contains("Rotation"))
		{
			Transform.Rotation.Roll = PrimitiveJson["Rotation"][0].get<float>();
			Transform.Rotation.Pitch = PrimitiveJson["Rotation"][1].get<float>();
			Transform.Rotation.Yaw = PrimitiveJson["Rotation"][2].get<float>();
		}

		if (PrimitiveJson.contains("Scale"))
		{
			Transform.Scale.X = PrimitiveJson["Scale"][0].get<float>();
			Transform.Scale.Y = PrimitiveJson["Scale"][1].get<float>();
			Transform.Scale.Z = PrimitiveJson["Scale"][2].get<float>();
		}

		if (!PrimitiveJson.contains("Type"))
		{
			continue;
		}

		FString TypeString = PrimitiveJson["Type"].get<FString>();

		// Text면 아래 property도 가져와서 함께 로드
		if (TypeString == "Text")
		{
			FTextMetaData TextData;
			TextData.Text = PrimitiveJson["Text"].get<FString>();
			TextData.FontPath = PrimitiveJson["FontPath"].get<FString>();
			TextData.FontPixelSize = PrimitiveJson["FontPixelSize"].get<int>();
			TextData.Color.X = PrimitiveJson["Color"][0].get<float>();
			TextData.Color.Y = PrimitiveJson["Color"][1].get<float>();
			TextData.Color.Z = PrimitiveJson["Color"][2].get<float>();
			TextData.Color.W = PrimitiveJson["Color"][3].get<float>();
			
			SceneData.TextDatas.insert(std::make_pair(UUID, TextData));
		
		}

		// Particle
		if (TypeString == "UVPlane")
		{
			FParticleMetaData ParticleData;
			ParticleData.ParticlePath = PrimitiveJson["ParticlePath"].get<FString>();

			SceneData.ParticleDatas.insert(std::make_pair(UUID, ParticleData));

		}

		SceneData.UUIDs.push_back(UUID);
		SceneData.Transforms.insert(std::make_pair(UUID, Transform));
		SceneData.Types.insert(std::make_pair(UUID, TypeString));
	}

	ReleaseFileOpenResource(FileSystem, Files, PWPath);
	return true;
}

bool FEditorFileUtils::SaveSceneWithFileBrowser(FSceneMetaData& SceneData)
{
	HRESULT Hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(Hr))
		return false;

	IFileSaveDialog* FileSystem;
	Hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&FileSystem));
	if (FAILED(Hr)) {
		CoUninitialize();
		return false;
	}

	COMDLG_FILTERSPEC SaveFilter[1];
	SaveFilter[0].pszName = L"Scene Files (*.Scene)";
	SaveFilter[0].pszSpec = L"*.Scene";
	Hr = FileSystem->SetFileTypes(1, SaveFilter);
	if (FAILED(Hr))
	{
		ReleaseFileSaveResource(FileSystem);
		return false;
	}
	Hr = FileSystem->SetFileTypeIndex(1);
	if (FAILED(Hr))
	{
		ReleaseFileSaveResource(FileSystem);
		return false;
	}

	FILEOPENDIALOGOPTIONS options;
	Hr = FileSystem->GetOptions(&options);
	if (FAILED(Hr))
	{
		ReleaseFileSaveResource(FileSystem);
		return false;
	}
	
	Hr = FileSystem->SetOptions(options | FOS_STRICTFILETYPES);
	if (FAILED(Hr))
	{
		ReleaseFileSaveResource(FileSystem);
		return false;
	}

	Hr = FileSystem->Show(NULL);
	if (Hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
	{
		ReleaseFileSaveResource(FileSystem);
		return false;
	}
	else if (FAILED(Hr))
	{
		ReleaseFileSaveResource(FileSystem);
		return false;
	}

	IShellItem* Files;
	Hr = FileSystem->GetResult(&Files);
	if (FAILED(Hr))
	{
		ReleaseFileSaveResource(FileSystem);
		return false;
	}

	PWSTR PWPath = nullptr;
	Hr = Files->GetDisplayName(SIGDN_FILESYSPATH, &PWPath);
	if (FAILED(Hr))
	{
		ReleaseFileSaveResource(FileSystem, Files);
		return false;
	}

	json Json;
	Json["Version"] = SceneData.Version;
	Json["NextUUID"] = SceneData.NextUUID;
	Json["Primitives"] = json::object();

	for (uint32 UUID : SceneData.UUIDs)
	{
		try
		{
			FTransform Transform = SceneData.Transforms.at(UUID);
			FString TypeString = SceneData.Types.at(UUID);

			json pJson;
			pJson["Location"] = { Transform.Location.X, Transform.Location.Y, Transform.Location.Z };
			pJson["Rotation"] = { Transform.Rotation.Roll, Transform.Rotation.Pitch, Transform.Rotation.Yaw };
			pJson["Scale"] = { Transform.Scale.X, Transform.Scale.Y, Transform.Scale.Z };
			pJson["Type"] = TypeString;

			// Text
			if (TypeString == "Text" && SceneData.TextDatas.count(UUID))
			{
				const FTextMetaData& TextData = SceneData.TextDatas.at(UUID);
				pJson["Text"] = TextData.Text;
				pJson["FontPath"] = TextData.FontPath;
				pJson["FontPixelSize"] = TextData.FontPixelSize;
				pJson["Color"] = { TextData.Color.X, TextData.Color.Y, TextData.Color.Z, TextData.Color.W };
			}

			// Particle
			if (TypeString == "UVPlane" && SceneData.ParticleDatas.count(UUID))
			{
				const FParticleMetaData& ParticleData = SceneData.ParticleDatas.at(UUID);
				pJson["ParticlePath"] = ParticleData.ParticlePath;
			}

			Json["Primitives"][std::to_string(UUID)] = pJson;
		}
		catch (const std::out_of_range& e)
		{
			continue;
		}
	}

	std::filesystem::path FSPath(PWPath);

	if (FSPath.extension() != ".Scene")
	{
		FSPath.replace_extension(".Scene");
	}

	FString Path = FSPath.string();
	std::ofstream File(Path);

	if (!File.is_open())
	{
		ReleaseFileSaveResource(FileSystem, Files, PWPath);
		return false;
	}

	File << Json.dump(4);

	File.close();

	ReleaseFileSaveResource(FileSystem, Files, PWPath);
	return true;
}

void FEditorFileUtils::ReleaseFileOpenResource(IFileOpenDialog* FileOpen, IShellItem* Items, PWSTR PWPath)
{
	if (PWPath)
	{
		CoTaskMemFree(PWPath);
	}

	if (Items)
	{
		Items->Release();
	}

	if (FileOpen)
	{
		FileOpen->Release();
	}

	CoUninitialize();
}

void FEditorFileUtils::ReleaseFileSaveResource(IFileSaveDialog* FileSave, IShellItem* Items, PWSTR PWPath)
{
	if (PWPath)
	{
		CoTaskMemFree(PWPath);
	}

	if (Items)
	{
		Items->Release();
	}

	if (FileSave)
	{
		FileSave->Release();
	}

	CoUninitialize();
}