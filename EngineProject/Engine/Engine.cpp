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
#include "Text/FontManager.h"

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

	// Font Manager
	LOG(Engine, Info, "Initialize FontManager...");
	FFontManager::GetInstance().SetDevice(Renderer->GetDevice());   // 주석 해제
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

		// BoundingBox 렌더링 전에 갱신
		World->UpdateTextComponentBounds(TextRenderer.get(), Renderer->GetDeviceContext());

		UpdateEditor(DeltaTime, Camera, VP);

		TQueue<FRenderPacket> RenderQueue;
		World->GatherRenderPackets(RenderQueue);
		FInputSystem::UpdateInputStates();

		Renderer->BeginFrame();

		// World
		if (Editor->GetShowFlags().IsSet(EShowFlagBits::Primitives))
			Renderer->RenderAll(RenderQueue, VP, Editor->GetSelectedTarget());

		// Editor
		Editor->OnRender(VP, Camera, Renderer.get());

		// ImGui 멀티뷰포트가 바꾼 Render Target 원복
		Renderer->BindMainRenderTarget();

		// Text Component Render
		World->RenderTextComponents(TextRenderer.get(), Renderer.get(), VP);

		FConsolePanel* Console = Editor->GetConsolePanel();

		if (Console && Console->HasActiveWorldText())
		{
			AActor* TargetActor = World->FindActorByUUID(Console->GetDebugTextTargetUUID());

			if (TargetActor)
			{
				FVector HeadOffset(0.0f, 0.0f, 2.0f); // Actor 살짝 위 (2.0f)
				FVector TextWorldPos = TargetActor->GetRootComponent()->GetTransform()->Location + HeadOffset;

				FVector CamRight = Camera->GetTransform()->GetRight();
				FVector CamUp = Camera->GetTransform()->GetUp();

				FDynamicFontAtlas* DebugAtlas = FFontManager::GetInstance().GetOrLoadAtlas("ThirdParty\\Pretendard-Regular.otf", 32);

				if (DebugAtlas)
				{
					TextRenderer->RenderTextWorld(
						Renderer.get(), Console->GetDebugTextString(),
						TextWorldPos, CamRight, CamUp,
						0.01f, 0.01f,
						FVector4(1.0f, 1.0f, 1.0f, 1.0f),
						VP,
						*DebugAtlas);
				}
			}
		}

		// DEBUG 용
		// if (Console && Console->ConsumeAtlasDumpRequest())
		// {
		// 	TextRenderer->SaveAtlasDebugBMP(Renderer.get(), "atlas_dump.bmp");
		// }

		// Gizmo는 항상 마지막에 그리기
		Editor->RenderGizmo(VP, Renderer.get());

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
	Editor->GetEditorSettings()->SaveEditorSetting();
	for (UObject* Object : GUObjectArray)
	{
		if (Object)
		{
			delete Object;
		}
	}

	Editor->Shutdown();
	Renderer->Shutdown();
}

void Engine::OnWindowResized(uint32 Width, uint32 Height)
{
	bIsResized = true;
}
