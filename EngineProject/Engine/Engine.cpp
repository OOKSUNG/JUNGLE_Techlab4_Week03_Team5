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


	FAtlasTextureManager::GetIntance().Init(Renderer.get());
	FAtlasTextureManager::GetIntance().LoadSampleAtlasTextures();
	//FTransform SpawnTransform;
	//AActor* StormEffect = World->SpawnActor<AActor>(&SpawnTransform);
	//UParticleSubUVComponent* StormSubUVComp = Cast<UParticleSubUVComponent>(StormEffect->AddPrimitiveComponent(EPrimitiveType::UVPlane, SpawnTransform));
	//if (StormSubUVComp)
	//{
	//	FAtlasTexture* Texture = FAtlasTextureManager::GetIntance().AtlasTextureMap["Storm"].get();
	//	Texture->SetParticleSamplerState();
	//	StormSubUVComp->SetTexture(Texture->GetTextureSRV(), Texture->GetSamplerState());
	//	StormSubUVComp->SetAtlasInfo(16, 1, 30.f);
	//}

	//AActor* AstralEffect = World->SpawnActor<AActor>(&SpawnTransform);
	//UParticleSubUVComponent* AstralSubUVComp = Cast<UParticleSubUVComponent>(AstralEffect->AddPrimitiveComponent(EPrimitiveType::UVPlane, SpawnTransform));
	//if (AstralSubUVComp)
	//{
	//	FFontManager::GetInstance().Init(Renderer.get());
	//	FFontManager::GetInstance().LoadAtlasTexture("Astral", "Font\\Astral.png");
	//	FAtlasTexture* Texture = FFontManager::GetInstance().AtlasTextureMap["Astral"].get();
	//	Texture->SetParticleSamplerState();
	//	AstralSubUVComp->SetTexture(Texture->GetTextureSRV(), Texture->GetSamplerState());
	//	AstralSubUVComp->SetAtlasInfo(16, 1, 10.f);
	//}

	//AActor* AxeEffect = World->SpawnActor<AActor>(&SpawnTransform);
	//UParticleSubUVComponent* AxeSubUVComp = Cast<UParticleSubUVComponent>(AxeEffect->AddPrimitiveComponent(EPrimitiveType::UVPlane, SpawnTransform));
	//if (AxeSubUVComp)
	//{
	//	FFontManager::GetInstance().Init(Renderer.get());
	//	FFontManager::GetInstance().LoadAtlasTexture("Axe", "Font\\Axe.png");
	//	FAtlasTexture* Texture = FFontManager::GetInstance().AtlasTextureMap["Axe"].get();
	//	Texture->SetParticleSamplerState();
	//	AxeSubUVComp->SetTexture(Texture->GetTextureSRV(), Texture->GetSamplerState());
	//	AxeSubUVComp->SetAtlasInfo(16, 1, 10.f);
	//}

	//AActor* CoreEffect = World->SpawnActor<AActor>(&SpawnTransform);
	//UParticleSubUVComponent* CoreSubUVComp = Cast<UParticleSubUVComponent>(CoreEffect->AddPrimitiveComponent(EPrimitiveType::UVPlane, SpawnTransform));
	//if (CoreSubUVComp)
	//{
	//	FFontManager::GetInstance().Init(Renderer.get());
	//	FFontManager::GetInstance().LoadAtlasTexture("Core", "Font\\Core.png");
	//	FAtlasTexture* Texture = FFontManager::GetInstance().AtlasTextureMap["Core"].get();
	//	Texture->SetParticleSamplerState();
	//	CoreSubUVComp->SetTexture(Texture->GetTextureSRV(), Texture->GetSamplerState());
	//	CoreSubUVComp->SetAtlasInfo(16, 1, 10.f);
	//}

	//AActor* MoonphaseEffect = World->SpawnActor<AActor>(&SpawnTransform);
	//UParticleSubUVComponent* MoonphaseSubUVComp = Cast<UParticleSubUVComponent>(MoonphaseEffect->AddPrimitiveComponent(EPrimitiveType::UVPlane, SpawnTransform));
	//if (MoonphaseSubUVComp)
	//{
	//	FFontManager::GetInstance().Init(Renderer.get());
	//	FFontManager::GetInstance().LoadAtlasTexture("Moonphase", "Font\\Moonphase.png");
	//	FAtlasTexture* Texture = FFontManager::GetInstance().AtlasTextureMap["Moonphase"].get();
	//	Texture->SetParticleSamplerState();
	//	MoonphaseSubUVComp->SetTexture(Texture->GetTextureSRV(), Texture->GetSamplerState());
	//	MoonphaseSubUVComp->SetAtlasInfo(16, 1, 10.f);
	//}

	//AActor* ExplosionEffect = World->SpawnActor<AActor>(&SpawnTransform);
	//UParticleSubUVComponent* ExplosionSubUVComp = Cast<UParticleSubUVComponent>(ExplosionEffect->AddPrimitiveComponent(EPrimitiveType::UVPlane, SpawnTransform));
	//if (ExplosionSubUVComp)
	//{
	//	FFontManager::GetInstance().Init(Renderer.get());
	//	FFontManager::GetInstance().LoadAtlasTexture("Explosion", "Font\\Explosion.png");
	//	FAtlasTexture* Texture = FFontManager::GetInstance().AtlasTextureMap["Explosion"].get();
	//	Texture->SetParticleSamplerState();
	//	ExplosionSubUVComp->SetTexture(Texture->GetTextureSRV(), Texture->GetSamplerState());
	//	ExplosionSubUVComp->SetAtlasInfo(3, 3, 5.0f);
	//}

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
		if (Editor->GetShowFlags().IsSet(EShowFlagBits::Primitives))
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

		// Gizmo는 editor 마지막에 그리기
		Editor->RenderGizmo(VP, Renderer.get());

		// UI Flush
		Editor->PresentUI();

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
