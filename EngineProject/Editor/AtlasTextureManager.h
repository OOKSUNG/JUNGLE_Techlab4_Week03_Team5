#pragma once
#include <string>
#include "Core/Containers.h"
#include "ObjectSystem/Property.h"
#include "AtlasTexture.h"


class FAtlasTextureManager
{
public:
	static FAtlasTextureManager& GetIntance()
	{
		static FAtlasTextureManager Instance;
		return Instance;
	}
	void Init(FRenderer* InRenderer);
	void LoadFontTexture(char const* KeyName, char const* FileName);

	TMap<FString, TSharedPtr<FAtlasTexture>> FontTextureMap;
	FAtlasTextureManager& operator=(FAtlasTextureManager&) = delete;
private:
	FRenderer* Renderer = nullptr;
	FAtlasTextureManager() = default;
	~FAtlasTextureManager() = default;
};