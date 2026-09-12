#include "EnginePCH.h"
#include "FontManager.h"

void FFontManager::LoadFontAtlas(char const* KeyName, char const* FileName, int AtlasCols = 16, int AtlasRows = 16)
{
	TSharedPtr<FFontAtlas> FontAtlas = MakeShared<FFontAtlas>();
	FontAtlas->Init(Renderer, FileName, AtlasCols, AtlasRows);
	FontAtlasMap[KeyName] = FontAtlas;
}

void FFontManager::Init(FRenderer* InRenderer)
{
	Renderer = InRenderer;
}