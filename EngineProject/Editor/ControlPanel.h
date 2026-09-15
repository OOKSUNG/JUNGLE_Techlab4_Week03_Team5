#pragma once
#include <format>
#include "EditorPanel.h"
#include "../Editor/EditorContext.h"
#include "ShowFlags.h"
#include "EditorSetting.h"


#include <functional>

using CallbackFunc = std::function<void()>;

class FControlPanel : public IEditorPanel
{
public:
	bool Init() override;
	void Tick(float DeltaTime)override;
	void OnRender() override;

	//inline void SetGizmo(FGizmo* InGizmo) { Gizmo = InGizmo; }
	inline void SetNewSceneCallback(CallbackFunc InCallback) { NewSceneCallback = InCallback; }
	inline void SetLoadSceneCallback(CallbackFunc InCallback) { LoadSceneCallback = InCallback; }
	inline void SetSaveSceneCallback(CallbackFunc InCallback) { SaveSceneCallback = InCallback; }

	inline void SetContext(FEditorContext InContext) { Context = InContext; }
	inline FEditorContext GetContext() { return Context; }

	float DeltaTime = 1.0f;
	// UWorld* World; // SpawnActor MainCamera;

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

	CallbackFunc NewSceneCallback = nullptr;
	CallbackFunc LoadSceneCallback = nullptr;
	CallbackFunc SaveSceneCallback = nullptr;
	
	// grid control
	void SetGridSpace(int32 Grid);
	float GetGridSpace() { return GridInterval; }

private:
	FEditorContext Context;
	const char* GridIntervals[5] = { "1", "5", "10", "50", "100" };
	int32 GridIntervalIndex = 0;
	int32 GridInterval = 1.0f;
};

