#include "EnginePCH.h"
#include "FontRenderer.h"


FFontRenderer::~FFontRenderer()
{

}

bool FFontRenderer::Init(FRenderer* InRenderer)
{
	Renderer = InRenderer;
	ID3D11DeviceContext* DeviceContext = Renderer->GetDeviceContext();
	FFontAtlas* DefaultFontAtlas = FFontManager::GetIntance().FontAtlasMap["Default"].get();

	
	return true;
}

void Begin()
{

}

void End()
{

}

void Shutdown()
{

}

void OnRender()
{
}