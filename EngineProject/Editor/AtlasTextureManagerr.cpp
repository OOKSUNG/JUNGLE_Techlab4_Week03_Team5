#include "EnginePCH.h"
#include "AtlasTextureManager.h"

void FAtlasTextureManager::LoadAtlasTexture(char const* KeyName, char const* FileName)
{
	TSharedPtr<FAtlasTexture> FontTexture = MakeShared<FAtlasTexture>();
	FontTexture->LoadTexture(Renderer, FileName);
	AtlasTextureMap[KeyName] = FontTexture;

}

void FAtlasTextureManager::Init(FRenderer* InRenderer)
{
	Renderer = InRenderer;
}

void FAtlasTextureManager::LoadSampleAtlasTextures()
{
	LoadAtlasTexture("Storm", "Font\\Storm.png");
	LoadAtlasTexture("Astral", "Font\\Astral.png");
	LoadAtlasTexture("Axe", "Font\\Axe.png");
	LoadAtlasTexture("Core", "Font\\Core.png");
	LoadAtlasTexture("Moonphase", "Font\\Moonphase.png");
}