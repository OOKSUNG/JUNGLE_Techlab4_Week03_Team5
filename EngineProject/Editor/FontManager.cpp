#include "EnginePCH.h"
#include "FontManager.h"

void FFontManager::LoadFontTexture(char const* KeyName, char const* FileName)
{
	TSharedPtr<FFontTexture> FontTexture = MakeShared<FFontTexture>();
	FontTexture->LoadTexture(Renderer, FileName);
	FontTextureMap[KeyName] = FontTexture;

}

void FFontManager::Init(FRenderer* InRenderer)
{
	Renderer = InRenderer;
}