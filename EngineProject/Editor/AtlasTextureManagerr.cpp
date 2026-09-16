#include "EnginePCH.h"
#include "AtlasTextureManager.h"

void FAtlasTextureManager::LoadFontTexture(char const* KeyName, char const* FileName)
{
	TSharedPtr<FAtlasTexture> FontTexture = MakeShared<FAtlasTexture>();
	FontTexture->LoadTexture(Renderer, FileName);
	FontTextureMap[KeyName] = FontTexture;

}

void FAtlasTextureManager::Init(FRenderer* InRenderer)
{
	Renderer = InRenderer;
}