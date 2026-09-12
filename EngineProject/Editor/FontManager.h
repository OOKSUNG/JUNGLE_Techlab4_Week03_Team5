#pragma once
#include "FontAtlas.h"
#include <string>
#include "Core/Containers.h"
#include "ObjectSystem/Property.h"


class FFontManager
{
public:
	static FFontManager& GetIntance()
	{
		static FFontManager Instance;
		return Instance;
	}
	void Init(FRenderer* InRenderer);
	FFontManager() = default;
	~FFontManager() = default;
	void LoadFontAtlas(char const* KeyName, char const* FileName, int AtlasCols, int AtlasRows);

	TMap<FString, TSharedPtr<FFontAtlas>> FontAtlasMap;
private:
	FRenderer* Renderer = nullptr;
};