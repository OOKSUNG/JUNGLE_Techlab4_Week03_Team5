   #include "EnginePCH.h"
#include "World.h"

#include "ObjectSystem/ObjectFactory.h"
#include "Core/EngineStatics.h"

#include "Camera/CameraComponent.h"
#include "Input/InputSystem.h"

#include "Collision/Ray.h"
#include "Core/FBoxBounds.h"

namespace
{
	FString PrimitiveTypeToString(EPrimitiveType Type)
	{
		switch (Type)
		{
		case EPrimitiveType::Sphere:
			return "Sphere";
			break;
		case EPrimitiveType::Cube:
			return "Cube";
			break;
		case EPrimitiveType::Cone:
			return "Cone";
			break;
		case EPrimitiveType::Plane:
			return "Plane";
			break;
		default:
			return "";
			break;
		}
	}

	EPrimitiveType FStringToPrimitiveType(const FString& string)
	{
		if (string == "Sphere")
		{
			return EPrimitiveType::Sphere;
		}
		if (string == "Cube")
		{
			return EPrimitiveType::Cube;
		}
		if (string == "Cone")
		{
			return EPrimitiveType::Cone;
		}
		if (string == "Plane")
		{
			return EPrimitiveType::Plane;
		}
		return EPrimitiveType::Cube;
	}
}

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
	NewActor->World = this;
	if (!NewActor) return nullptr;

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

bool UWorld::NewScene()
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

bool UWorld::SaveScene(FSceneMetaData& SceneData)
{
	SceneData.Version = 1;
	SceneData.NextUUID = FEngineStatics::NextUUID;

	for (AActor* Actor : Actors)
	{
		USceneComponent* Primitive = Actor->GetRootComponent();
		
		if (Primitive == nullptr)
		{
			continue;
		}

		uint32 UUID = Primitive->GetUUID();
		SceneData.UUIDs.push_back(UUID);

		const FTransform* Transform = Primitive->GetTransform();
		SceneData.Transforms.insert(std::make_pair(UUID, *Transform));

		if (Cast<UPrimitiveComponent>(Primitive))
		{
			SceneData.Types.insert(std::make_pair(UUID, PrimitiveTypeToString(Cast<UPrimitiveComponent>(Primitive)->GetType())));
		}
		else if (Cast<UCameraComponent>(Primitive))
		{
			SceneData.Types.insert(std::make_pair(UUID, "Camera"));
		}
		else
		{
			SceneData.Types.insert(std::make_pair(UUID, "Other"));
		}
	}

	return true;
}

bool UWorld::LoadScene(const FSceneMetaData& Data)
{
	FEngineStatics::NextUUID = Data.NextUUID;

	if (Data.UUIDs.empty())
	{
		return true;
	}

	for (auto& UUID : Data.UUIDs)
	{
		try 
		{
			FTransform Transform = Data.Transforms.at(UUID);
			FString TypeString = Data.Types.at(UUID);

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
				continue;

			EPrimitiveType Type = FStringToPrimitiveType(TypeString);

			// 액터 스폰
			AActor* Actor = SpawnActor(AActor::StaticClass(), &Transform);
			Actor->AddPrimitiveComponent(Type, Transform);
			Actor->SetUUID(UUID);
		}
		catch (const std::out_of_range& e)
		{
			LOG(World, Error, "Failed Parsing Actor Information for: {}", UUID);
			continue;
		}
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

// UUID로 Actor 찾기
AActor* UWorld::FindActorByUUID(uint32 InUUID) const
{
	for (AActor* Actor : Actors)
	{
		if (Actor->GetUUID() == InUUID)
		{
			return Actor;
		}
			
	}
	return nullptr;
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