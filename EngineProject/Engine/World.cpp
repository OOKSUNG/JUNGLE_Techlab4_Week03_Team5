   #include "EnginePCH.h"
#include "World.h"

#include "ObjectSystem/ObjectFactory.h"
#include "Core/EngineStatics.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Input/InputSystem.h"

#include "Collision/Ray.h"
#include "Core/FBoxBounds.h"



//UPrimitiveComponent* UWorld::SpawnPrimitive(FClass* Class)
//{
//	UPrimitiveComponent* Component = FObjectFactory::ConstructObject<UPrimitiveComponent>();
//	Primitives.push_back(Component);
//	return Component;
//}

UWorld::~UWorld()
{
}

bool UWorld::Init()
{

	// Spawn Actor로 카메라 생성하고 세팅하기
	ACameraActor* GetCamera = SpawnActor<ACameraActor>(nullptr);

	// Camera 초기 위치 수정
	GetCamera->GetCameraComponent()->SetLocation(FVector(-8.0f, -0.1f, 2.0f));

	FString NewName =  FString("Camera_") + std::to_string(GetCamera->GetUUID());

	GetCamera->SetFName(NewName);

	if (GetCamera)
	{
		SetMainCamera(GetCamera);
		return true;
	}


	return false;
}

AActor* UWorld::SpawnActor(UClass* Class, const FTransform* UserTransformPtr)
{
	if (!Class) return nullptr;
	if (!Class->IsChildOf(AActor::StaticClass())) return nullptr;
	const FTransform UserTransform = UserTransformPtr ? *UserTransformPtr : FTransform::Identity;
	UObject* NewObject= FObjectFactory::ConstructObject(Class);
	AActor* NewActor = Cast<AActor>(NewObject);

	if (!NewActor) return nullptr;
	NewActor->World = this;

	if (!NewActor->GetRootComponent())
	{
		USceneComponent* DefaultRoot = FObjectFactory::ConstructObject<USceneComponent>();
		DefaultRoot->SetTransform(UserTransform);
		NewActor->SetRootComponent(DefaultRoot);
	}

	Actors.push_back(NewActor);
	BeginPlayList.push(NewActor);
	return NewActor;
}

void UWorld::Tick(float DeltaTime)
{
	while (!BeginPlayList.empty())
	{
		BeginPlayList.front()->BeginPlay();
		BeginPlayList.pop();
	}

	for (AActor* Actor : Actors)
	{
		Actor->Tick(DeltaTime);
	}
}

void UWorld::OnRender(FRenderer* Renderer)
{
	// Rendering
}

void UWorld::ClearScene()
{
	PrimitiveComponents.clear();
	while (!BeginPlayList.empty()) BeginPlayList.pop();

	for (AActor* Actor : Actors) delete Actor;
	Actors.clear();
	MainCamera = nullptr;
}

bool UWorld::NewScene(const FString& Path)
{
	ACameraActor* GetCamera = SpawnActor<ACameraActor>(nullptr);
	// Camera 초기 위치 수정
	GetCamera->GetCameraComponent()->SetLocation(FVector(-8.0f, -0.1f, 2.0f));
	
	if (GetCamera)
	{
		SetMainCamera(GetCamera);
	}
	MainCamera = GetCamera;

	FEngineStatics::NextUUID = 0;

	return true;
}

bool UWorld::SaveScene(const FString& Path)
{
	json Json;
	Json.dump(4);
	Json["Version"] = 1;
	Json["NextUUID"] = FEngineStatics::NextUUID;
	Json["Primitives"] = json::object();

	for (AActor* Actor : Actors)
	{

		USceneComponent* Primitive = Actor->GetRootComponent();

		if (Primitive == nullptr)
		{
			continue;
		}

		json pJson;

		const FTransform* Transform = Primitive->GetTransform();

		pJson["Location"] = { Transform->Location.X, Transform->Location.Y, Transform->Location.Z };
		pJson["Rotation"] = { Transform->Rotation.Roll, Transform->Rotation.Pitch, Transform->Rotation.Yaw };
		pJson["Scale"] = { Transform->Scale.X, Transform->Scale.Y, Transform->Scale.Z };

		if (Cast<UPrimitiveComponent>(Primitive))
		{
			pJson["Type"] = PrimitiveTypeToString(Cast<UPrimitiveComponent>(Primitive)->GetType());
		}
		else if (Cast<UCameraComponent>(Primitive))
		{
			pJson["Type"] = "Camera";
		}
		else
		{
			pJson["Type"] = "Other";
		}


		Json["Primitives"][std::to_string(Primitive->GetUUID())] = pJson;

	}

	std::filesystem::create_directories("Scene");

	FString FullPath = "Scene/" + Path + ".Scene";
	std::ofstream File(FullPath);

	if (!File.is_open())
	{
		return false;
	}

	File << Json.dump(4);

	File.close();

	std::cout << Json.dump(4);
	return true;
}

bool UWorld::LoadScene(const FString& Path)
{
	std::filesystem::create_directories("Scene");

	FString FullPath = "Scene/" + Path + ".Scene";
	std::ifstream File(FullPath);

	if (!std::filesystem::exists(FullPath))
	{
		LOG(World, Warning, "{} is Not Exist!", FullPath);
		return false;
	}

	// ClearScene();

	if (!File.is_open())
	{
		return false;
	}

	json Json;

	try
	{
		File >> Json;
	}
	catch (const json::parse_error&)
	{
		File.close();
		return false;
	}

	File.close();

	if (!Json.contains("Version"))
	{
		return false;
	}

	if (Json["Version"] != 1)
	{
		return false;
	}

	if (Json.contains("NextUUID"))
	{
		FEngineStatics::NextUUID = Json["NextUUID"].get<uint64>();
	}

	if (!Json.contains("Primitives"))
	{
		return true;
	}

	for (auto& [UUIDString, PrimitiveJson] : Json["Primitives"].items())
	{
		uint64 UUID = std::stoull(UUIDString);

		FTransform Transform;

		if (PrimitiveJson.contains("Location"))
		{
			Transform.Location.X = PrimitiveJson["Location"][0].get<float>();
			Transform.Location.Y = PrimitiveJson["Location"][1].get<float>();
			Transform.Location.Z = PrimitiveJson["Location"][2].get<float>();
		}

		if (PrimitiveJson.contains("Rotation"))
		{
			Transform.Rotation.Roll = PrimitiveJson["Rotation"][0].get<float>();
			Transform.Rotation.Pitch = PrimitiveJson["Rotation"][1].get<float>();
			Transform.Rotation.Yaw = PrimitiveJson["Rotation"][2].get<float>();
		}

		if (PrimitiveJson.contains("Scale"))
		{
			Transform.Scale.X = PrimitiveJson["Scale"][0].get<float>();
			Transform.Scale.Y = PrimitiveJson["Scale"][1].get<float>();
			Transform.Scale.Z = PrimitiveJson["Scale"][2].get<float>();
		}

		if (!PrimitiveJson.contains("Type"))
		{
			continue;
		}


		FString TypeString = PrimitiveJson["Type"].get<FString>();

		if (TypeString == "Camera")
		{
			// 현재 카메라 delete하고 새로 생성
			ACameraActor* GetCamera = SpawnActor<ACameraActor>(nullptr);
			if (GetCamera)
			{
				SetMainCamera(GetCamera);
			}
			MainCamera = GetCamera;
			MainCamera->GetRootComponent()->SetTransform(Transform);

			continue;
		}

		if (TypeString == "Other")
		{
			continue;
		}

		EPrimitiveType Type = FStringToPrimitiveType(TypeString);


		// 액터 스폰
		AActor* Actor = SpawnActor(AActor::StaticClass(), &Transform);
		Actor->AddPrimitiveComponent(Type, Transform);
		Actor->SetUUID(UUID);


	}

	return true;
}

void UWorld::GatherRenderPackets(TQueue<FRenderPacket>& RenderQueue)
{
	for (UPrimitiveComponent* Primitive : PrimitiveComponents)
	{
		if (Primitive)
			Primitive->SubmitToRenderQueue(RenderQueue);
	}
}

UPrimitiveComponent* UWorld::GetPickingPrimitive(uint32 ScreenW, uint32 ScreenH)
{
	FRay Ray = MainCamera->GetCameraComponent()->DeProjection(FInputSystem::GetMouseX(), FInputSystem::GetMouseY(), ScreenW, ScreenH);

	UPrimitiveComponent* PickingPrimitive = nullptr;
	float MinT{ FLT_MAX };

	for (UPrimitiveComponent* Primitive : PrimitiveComponents)
	{
		if (!Primitive) continue;
		const FBoxBounds& Bounds = Primitive->GetBounds();
		
		if (AABBInspection(Ray, Bounds, MinT) && TriangleInspection(Ray, *Primitive, MinT))
		{
			PickingPrimitive = Primitive;
		}
	}

	return PickingPrimitive;
}

bool UWorld::AABBInspection(const FRay& Ray, const FBoxBounds& Bounds, float& MinT)
{
	float RayT{};

	return (RayIntersectsAABB(Ray, Bounds, RayT) && RayT <= MinT);
}

bool UWorld::TriangleInspection(const FRay& Ray, const UPrimitiveComponent& Primitive, float& MinT)
{
	float RayT{};
	// ray를 로컬공간으로
	FMatrix invWorld = Primitive.GetWorldMatrix().Inverse();
	FVector4 LocalRayOrigin = invWorld.TransformPosition(Ray.Origin);
	FVector4 LocalRayDir = invWorld.TransformVector(Ray.Direction);

	FRay LocalRay{};
	LocalRay.Origin.X = LocalRayOrigin.X;
	LocalRay.Origin.Y = LocalRayOrigin.Y;
	LocalRay.Origin.Z = LocalRayOrigin.Z;

	LocalRay.Direction.X = LocalRayDir.X;
	LocalRay.Direction.Y = LocalRayDir.Y;
	LocalRay.Direction.Z = LocalRayDir.Z;

	const FMeshData& Mesh = Primitive.GetMeshData();

	// Broad Phase 통과하면 뮐러-트럼보르 알고리즘 수행
	for (uint32 i = 0; i + 2 < Mesh.Indices.size(); i += 3)
	{
		FVector vertices[3]{};	// 3 vertex
		for (uint32 j = 0; j < 3; ++j)
		{
			uint32 index = Mesh.Indices[i + j];

			vertices[j].X = Mesh.Vertices[index].Position.X;
			vertices[j].Y = Mesh.Vertices[index].Position.Y;
			vertices[j].Z = Mesh.Vertices[index].Position.Z;
		}

		if (!RayIntersectsTriangle(LocalRay, vertices[0], vertices[1], vertices[2], RayT))
		{
			continue;
		}

		if (RayT < MinT)
		{
			MinT = RayT;
			return true;
		}
	}

	return false;
}


// 카메라 관련 추가

void UWorld::SetMainCamera(ACameraActor* Camera)
{
	MainCamera = Camera;
}

ACameraActor* UWorld::GetMainCamera() const
{
	return MainCamera;
}