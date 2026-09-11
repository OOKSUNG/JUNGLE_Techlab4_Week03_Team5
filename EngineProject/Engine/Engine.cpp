#include "EnginePCH.h"

#include "Engine.h"
#include "Core/EngineStatics.h"
#include "Core/EngineTimer.h"
#include "Input/InputSystem.h"

#include "ObjectSystem/ObjectFactory.h"

#include "Render/GeometryGenerator.h"

#include "World.h"

#include "Render/Renderer.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Engine/ResourceManager.h"
#include "Editor/EditorUI.h"


bool Engine::Init(HINSTANCE hInstance)
{
	// UI가 사용할 정보 저장
	/*FEditorContext Context;
	Context.World = World;*/
	

	// UI 생성 및 초기화, 컨텍스트 전달
	/*EditorUI = MakeUnique<FEditorUI>();

	if (!EditorUI->Init(Context))
	{
		return false;
	}
	ConsolePanel = EditorUI->GetEditorPanel<FConsolePanel>();*/

	//LOG(Info, "Success!");

	//// 씬 클리어 호출 시 콜백 함수
	//EditorUI->GetEditorPanel<FControlPanel>()->SetSceneClearCallback([&]() {
	//	Gizmo->SetTarget(nullptr);
	//	Outline->SetTarget(nullptr);
	//	EditorUI->GetEditorPanel<FPropertyPanel>()->SetTarget(nullptr);
	//	}
	//);

		//LOG(Info, "Success!");

	// Create Main Window
	// LOG(Info, "Create Main Window...");
	MainWindow = MakeUnique<Window>();
	if (!MainWindow->Create(hInstance, 1280, 720, L"Engine"))
	{
		// LOG(Error, "Failed To Create Main Window!");
		return false;
	}
	// LOG(Info, "Success!");

	// LOG(Info, "Initialize Renderer...");
	Renderer = MakeUnique<FRenderer>();
	if (!Renderer->Init(MainWindow->GetHandle()))
	{
		// LOG(Error, "Failed To Initialize Renderer!");
		return false;
	}
	// LOG(Info, "Success!");



	// LOG(Info, "Initialize ResourceManager...");
	FResourceManager::GetInstance().Init(Renderer.get());
	// LOG(Info, "Success!");



	/*LOG(Info, "Initialize ImGui...");
	ImGuiRenderer = MakeUnique<FImGuiRenderer>();
	if (!ImGuiRenderer->Init(MainWindow->GetHandle(), Renderer->GetDevice(), Renderer->GetDeviceContext()))
	{
		LOG(Error, "Failed To Initialize ImGui!");
		return false;
	}
	LOG(Info, "Success!");*/

	//GridRenderer = MakeUnique<FGridRenderer>();
	//GridRenderer->Init(Renderer.get());

	//GizmoRenderer = MakeUnique<FGizmoRenderer>();
	//GizmoRenderer->Init(Renderer.get());

	//Gizmo = MakeUnique<FGizmo>();

	//// Resource Manager 

	//// OutLine -> 에디터 안으로 넣음
	//OutlineRenderer = MakeUnique<FOutlineRenderer>();
	//OutlineRenderer->Init(Renderer.get());

	//Outline = MakeUnique<FOutline>();

	// Do Sth
	World = FObjectFactory::ConstructObject<UWorld>();
	if (!World->Init())
	{
		return false;
	}

	Editor = MakeUnique<FEditor>();

	if (!Editor->Init(Renderer.get(), World, MainWindow->GetHandle()))
	{
		return false;
	}

	bIsRunning = true;

	return true;
}

void Engine::Run()
{
	EngineTimer::Init(); // return bool

	LOG(Info, "{}", "Hello, World!");

	FMatrix Mat;
	Mat.SetIdentity();

	FMatrix VP;

	while (bIsRunning)
	{
		EngineTimer::Tick();
		float DeltaTime = EngineTimer::GetDeltaTime();

		//Check Swapchain Resize
		MainWindow->ProcessMessage(bIsRunning);
		HandleResize();

		//Update World / UI
		World->Tick(DeltaTime);

		ACameraActor* MainCamera = World->GetMainCamera();
		UCameraComponent* Camera = MainCamera->GetCameraComponent();
		VP = Camera->GetViewProjectionMatrix();

		UpdateEditor(DeltaTime, Camera, VP);

		TQueue<FRenderPacket> RenderQueue;
		World->GatherRenderPackets(RenderQueue);
		FInputSystem::UpdateInputStates();
	
		Renderer->BeginFrame();

		// World
		Renderer->RenderAll(RenderQueue, VP);

		// Editor
		Editor->OnRender(VP, Camera, Renderer.get());

		Renderer->EndFrame();
	
	}
}

void Engine::HandleResize()
{
	if (MainWindow->CheckResized())
	{
		Renderer->Resize(MainWindow->GetWidth(), MainWindow->GetHeight());
		World->GetMainCamera()->GetCameraComponent()->SetAspectRatio((float)MainWindow->GetWidth() / MainWindow->GetHeight());
	}
}

void Engine::UpdateEditor(float DeltaTime, UCameraComponent* Camera, FMatrix VP)
{
	uint32 WinWidth = MainWindow->GetWidth();
	uint32 WinHeight = MainWindow->GetHeight();

	Editor->Update(DeltaTime, Camera, VP, WinWidth, WinHeight);
}

void Engine::Shutdown()
{
	for (UObject* Object : GUObjectArray)
		delete Object;
	Editor->Shutdown();
	Renderer->Shutdown();
}

void Engine::OnWindowResized(uint32 Width, uint32 Height)
{
	bIsResized = true;
}
