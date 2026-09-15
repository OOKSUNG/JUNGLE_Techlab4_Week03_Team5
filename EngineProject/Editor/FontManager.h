#pragma once
#include <string>
#include "Core/Containers.h"
#include "ObjectSystem/Property.h"
#include "FontTexture.h"


class FFontManager
{
public:
	static FFontManager& GetIntance()
	{
		static FFontManager Instance;
		return Instance;
	}
	void Init(FRenderer* InRenderer);
	void LoadFontTexture(char const* KeyName, char const* FileName);
	
	TMap<FString, TSharedPtr<FFontTexture>> FontTextureMap;
	FFontManager& operator=(FFontManager&) = delete;
private:
	FRenderer* Renderer = nullptr;
	FFontManager() = default;
	~FFontManager() = default;
};