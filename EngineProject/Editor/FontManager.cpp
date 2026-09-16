#include "EnginePCH.h"
#include "FontManager.h"

void FEditorFontManager::LoadFontTexture(char const* KeyName, char const* FileName)
{
	TSharedPtr<FFontTexture> FontTexture = MakeShared<FFontTexture>();
	FontTexture->LoadTexture(Renderer, FileName);
	FontTextureMap[KeyName] = FontTexture;

}

void FEditorFontManager::Init(FRenderer* InRenderer)
{
	Renderer = InRenderer;
}