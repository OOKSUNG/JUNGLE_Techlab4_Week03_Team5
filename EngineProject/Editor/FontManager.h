#pragma once
#include <string>
#include "Core/Containers.h"
#include "ObjectSystem/Property.h"
#include "FontTexture.h"


class FEditorFontManager
{
public:
	static FEditorFontManager& GetIntance()
	{
		static FEditorFontManager Instance;
		return Instance;
	}
	void Init(FRenderer* InRenderer);
	void LoadFontTexture(char const* KeyName, char const* FileName);

	TMap<FString, TSharedPtr<FFontTexture>> FontTextureMap;
	FEditorFontManager& operator=(FEditorFontManager&) = delete;
private:
	FRenderer* Renderer = nullptr;
	FEditorFontManager() = default;
	~FEditorFontManager() = default;
};