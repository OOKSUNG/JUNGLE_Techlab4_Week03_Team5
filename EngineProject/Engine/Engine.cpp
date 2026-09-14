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
	// Create Main Window
	LOG(Engine, Info, "Create Main Window...");
	MainWindow = MakeUnique<Window>();
	if (!MainWindow->Create(hInstance, 1280, 720, L"Engine"))
	{
		LOG(Engine, Error, "Failed To Create Main Window!");
		return false;
	}
	LOG(Engine, Info, "Success!");

	LOG(Engine, Info, "Initialize Renderer...");
	Renderer = MakeUnique<FRenderer>();
	if (!Renderer->Init(MainWindow->GetHandle()))
	{
		LOG(Engine, Error, "Failed To Initialize Renderer!");
		return false;
	}
	LOG(Engine, Info, "Success!");

	LOG(Engine, Info, "Initialize ResourceManager...");
	if (!FResourceManager::GetInstance().Init(Renderer.get()))
	{
		LOG(Engine, Info, "Failed To Initialize ResourceManager");
		return false;
	}
	LOG(Engine, Info, "Success!");

	// Do Sth
	LOG(Engine, Info, "Initialize World...");
	World = FObjectFactory::ConstructObject<UWorld>();
	if (!World->Init())
	{
		LOG(Engine, Info, "Failed To Initialize World");
		return false;
	}
	LOG(Engine, Info, "Success!");
	Editor = MakeUnique<FEditor>();

	// Text Renderer
	TextRenderer = MakeUnique<FTextRenderer>();

	LOG(Engine, Info, "Initialize TextRenderer...");
	if (!TextRenderer->Init(Renderer.get(), L"ThirdParty\\Pretendard-Regular.otf", 32))
	{
		LOG(Engine, Info, "Failed To Initialize Text");
		return false;
	}

	LOG(Engine, Info, "Success!");
	Editor = MakeUnique<FEditor>();

	LOG(Engine, Info, "Initialize Editor...");
	if (!Editor->Init(Renderer.get(), World, MainWindow->GetHandle()))
	{
		LOG(Engine, Info, "Failed To Initialize Editor");
		return false;
	}
	LOG(Engine, Info, "Success!");

	bIsRunning = true;

	return true;
}

void Engine::Run()
{
	EngineTimer::Init(); // return bool

	LOG(Engine, Info, "Hello, World!");

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
		if (Editor->GetShowFlags().IsSet(EShowFlagBits::Primitives))
			Renderer->RenderAll(RenderQueue, VP, Editor->GetSelectedTarget());
			
		// Text
		TextRenderer->RenderText(
			Renderer.get(), "안녕하세요 크래프톤 정글 게임테크랩 하하하",
			FVector2(0.0f, 0.0f), FVector4(1.0f, 1.0f, 1.0f, 1.0f),
			MainWindow->GetWidth(), MainWindow->GetHeight());
		
		// 한글 렌더링 아틀라스 보고 싶으면 주석 해제하고 해보세요
		// atlas_dump.bup 에 저장됩니다
		static bool bDumpedOnce = false;
		if (!bDumpedOnce)
		{
			TextRenderer->SaveAtlasDebugBMP(Renderer.get(), "atlas_dump.bmp");
			bDumpedOnce = true;
		}

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
	FEditorSettings::Get().SaveEditorSetting();
	for (UObject* Object : GUObjectArray)
		delete Object;
	Editor->Shutdown();
	Renderer->Shutdown();
}

void Engine::OnWindowResized(uint32 Width, uint32 Height)
{
	bIsResized = true;
}
