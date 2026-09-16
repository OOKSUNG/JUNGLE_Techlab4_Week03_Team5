#pragma once

#include "ObjectSystem/Object.h"
#include "GameFramework/Actor.h"
#include "Component/PrimitiveComponent.h"
#include "Math/Transform.h"
#include "Render/Renderer.h"
#include "Camera/CameraActor.h"
#include "Engine/SceneMetaData.h"

class ACameraActor;
class UTextComponent;
class FTextRenderer;

class UWorld : public UObject
{
	DECLARE_CLASS(UWorld, UObject)
public:
	UWorld() = default;
	virtual ~UWorld();

	bool Init();
	/*UPrimitiveComponent* SpawnPrimitive(FClass* Class);*/
	AActor* SpawnActor(UClass* Class, const FTransform* Transform);

	template <class T>
	T* SpawnActor(const FTransform* Transform)
	{
		return CastChecked<T>(SpawnActor(T::StaticClass(), Transform));
	}

	void Tick(float DeltaTime);

	void OnRender(FRenderer* Renderer);

	void ClearScene();
	bool NewScene();
	bool SaveScene(FSceneMetaData& SceneData);
	bool LoadScene(const FSceneMetaData& Data);

	inline void AddPrimitive(UPrimitiveComponent* Primitive) { PrimitiveComponents.push_back(Primitive); }
	inline void AddTextComponent(UTextComponent* TextComponent) { TextComponents.push_back(TextComponent); }

	void GatherRenderPackets(TQueue<FRenderPacket>& RenderQueue);
	void RenderTextComponents(FTextRenderer* TextRenderer, FRenderer* Renderer, const FMatrix& VP);
	void UpdateTextComponentBounds(FTextRenderer* TextRenderer, ID3D11DeviceContext* Context);

	// UPrimitiveComponent* GetPickingPrimitive(uint32 ScreenW, uint32 ScreenH);
	AActor* GetPickingPrimitive(uint32 ScreenW, uint32 ScreenH);
	AActor* FindActorByUUID(uint32 InUUID) const;

	// 카메라 세터, 게터
	void SetMainCamera(ACameraActor* Camera);
	ACameraActor* GetMainCamera() const;

	int32 GetActorNum() const { return (int32)Actors.size(); }


	// TArray<AActor*> GetActors() const { return Actors; }

	TArray<UPrimitiveComponent*> GetPrimitiveComponents() { return (PrimitiveComponents); };

	const TArray<AActor*>& GetActors() const { return Actors; };
private:
	TArray<AActor*> Actors;

	TQueue<AActor*> BeginPlayList;
	
	TArray<UPrimitiveComponent*> PrimitiveComponents;

	//카메라 추가 
	ACameraActor* MainCamera = nullptr;

	// 피킹 프리미티브
	bool AABBInspection(const FRay& Ray, const FBoxBounds& Bounds, float& MinT);
	bool TriangleInspection(const FRay& Ray, const UPrimitiveComponent& Primitive, float& MinT);

	// Text Components
	TArray<UTextComponent*> TextComponents;
};

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
		case EPrimitiveType::Text:
			return "Text";
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
		if (string == "Text")
		{
			return EPrimitiveType::Text;
		}
		if (string == "UVPlane")
		{
			return EPrimitiveType::UVPlane;
		}
		return EPrimitiveType::Cube;
	}
}
