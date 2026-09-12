#pragma once
#include <Windows.h>
#include <d3d11.h>
#include "Render/Renderer.h"
#include "FontManager.h"
#include "FontAtlas.h"

class FFontRenderer
{
public:
	FFontRenderer() = default;
	~FFontRenderer();

	bool Init(FRenderer* InRenderer);
	void Begin();
	void End();
	void Shutdown();
	void OnRender();
private:
	FRenderer* Renderer;
};