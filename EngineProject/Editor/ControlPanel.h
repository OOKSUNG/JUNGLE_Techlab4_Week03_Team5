#pragma once
#include <format>
#include "EditorPanel.h"
#include "../Editor/EditorContext.h"
#include "ShowFlags.h"
#include "EditorSetting.h"


#include <functional>

using SceneClearCallback = std::function<void()>;

class FControlPanel : public IEditorPanel
{
public:
	bool Init() override;
	void Tick(float DeltaTime)override;
	void OnRender() override;

	//inline void SetGizmo(FGizmo* InGizmo) { Gizmo = InGizmo; }
	inline void SetSceneClearCallback(SceneClearCallback InCallback) { Callback = InCallback; }

	inline void SetContext(FEditorContext InContext) { Context = InContext; }
	inline FEditorContext GetContext() { return Context; }

	float DeltaTime = 1.0f;
	// UWorld* World; // SpawnActor MainCamera
	char SceneName[128] = "";

	FEngineShowFlags* ShowFlags = nullptr;


	void AddActor(EPrimitiveType Type);

	int32 SelectedIndex = 0;
	int32 ActorNum = 0;

	const char* Items[4] =
	{
		"Sphere",
		"Cube",
		"Cone",
		"Plane",
	};
	// View Mode
	FRenderer* Renderer;
	inline void SetRenderer(FRenderer* InRenderer) { Renderer = InRenderer; };
	int32 ViewModeIndex = 1; 	// Unlit

	// Gizmo

	int32 GizmoSelectedIndex = 0;

	const char* GizmoItems[3] =
	{
		"Location",
		"Rotation",
		"Scale"
	};

	int32 SpaceSelectedIndex = 0;

	const char* SpaceItems[2] =
	{
		"Local",
		"World"
	};

	SceneClearCallback Callback = nullptr;
	
	// grid control
	void SetGridSpace(float Grid) { GridSpace = Grid; }
	float GetGridSpace() { return GridSpace; }

private:
	FEditorContext Context;
	float GridSpace = 10.0f;
};

