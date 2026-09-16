#include "EnginePCH.h"
#include "FontManager.h"
#include <filesystem>

FFontManager& FFontManager::GetInstance()
{
    static FFontManager Instance;
    return Instance;
}

FDynamicFontAtlas* FFontManager::GetOrLoadAtlas(const FString& FontPath, int FontPixelSize, int AtlasSize)
{
    if (!Device)
    {
        return nullptr;
    }

    // Font + Size를 Font Atlas의 Key로 저장
    FString Key = FontPath + "_" + std::to_string(FontPixelSize);
    
    auto it = AtlasCache.find(Key);
    if (it != AtlasCache.end())
    {
        // 찾으면 Font Atlas 반환
        return it->second.get();
    }

    std::wstring WideFontPath(FontPath.begin(), FontPath.end());
    
    // 없으면 아틀라스 새로 만들기
    TSharedPtr<FDynamicFontAtlas> NewAtlas = MakeShared<FDynamicFontAtlas>();

    if (!NewAtlas->Init(Device, WideFontPath.c_str(), FontPixelSize, AtlasSize))
    {
        return nullptr;
    }

    AtlasCache[Key] = NewAtlas;
    return NewAtlas.get();

}

const TArray<FString>& FFontManager::GetAvailableFontPaths()
{
    if (CachedFontPaths.empty())
    {
        namespace fs = std::filesystem;
        const FString Directory = "ThirdParty";

        if (fs::exists(Directory))
        {
            for (const auto& Entry : fs::directory_iterator(Directory))
            {
                if (!Entry.is_regular_file())
                    continue;
                
                // 확장자가 otf or ttf 면 path list에 push
                FString Ext = Entry.path().extension().string();
                if (Ext == ".otf" || Ext == ".ttf")
                {
                    CachedFontPaths.push_back(Entry.path().string());
                }
            }
        }
    }
    return CachedFontPaths;
}