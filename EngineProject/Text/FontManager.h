#pragma once

#include <d3d11.h>
#include "FontAtlas.h"

class FFontManager
{
private:
    FFontManager() = default;

    FFontManager(const FFontManager&) = delete;
    FFontManager& operator=(const FFontManager&) = delete;

    TMap<FString, TSharedPtr<FDynamicFontAtlas>> AtlasCache;
    ID3D11Device* Device = nullptr;
    TArray<FString> CachedFontPaths;

public:
    static FFontManager& GetInstance();
    void SetDevice(ID3D11Device* InDevice) { Device = InDevice; }
    FDynamicFontAtlas* GetOrLoadAtlas(const FString& FontPath, int FontPixelSize, int AtlasSize = 1024);
    const TArray<FString>& GetAvailableFontPaths();     // Font 선택
};