#pragma once

#include "Core/Window.h"
#include "String.h"
#include "Core/Types.h"
#include "Core/Containers.h"

#include "World.h"
#include "Render/Renderer.h"
#include "ImGuiRenderer.h"
#include "Editor/GridRenderer.h"
#include "Editor/GizmoRenderer.h"
#include "Text/TextRenderer.h"

#include "Editor/EditorUI.h"
#include "Editor/ConsolePanel.h"
#include "Editor/PropertyPanel.h"
#include "Editor/ControlPanel.h"

#include "Editor/Outline.h"
#include "Editor/OutLineRenderer.h"
#include "Editor/Editor.h"

#include "Editor/EditorSetting.h"

class Engine
{
public:
	bool Init(HINSTANCE hInstance);
	void Run();
	void Shutdown();
	void HandleResize();
	void UpdateEditor(float DeltaTime, UCameraComponent* Camera, FMatrix VP);
	void OnWindowResized(uint32 Width, uint32 Height);

private:
	bool bIsRunning = false;
	bool bIsResized = false;
	
	TUniquePtr<Window> MainWindow;

	UWorld* World;

	TUniquePtr<FRenderer> Renderer;
	TUniquePtr<FTextRenderer> TextRenderer;

	TUniquePtr<FEditor> Editor;

	TSharedPtr<FVertexBuffer> vb;
	TSharedPtr<FIndexBuffer> ib;
	TSharedPtr<FMesh> Mesh;
	TSharedPtr<FShader> Shader;
};

