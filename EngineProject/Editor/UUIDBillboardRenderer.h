#pragma once
#include "Component/PrimitiveComponent.h"
#include "Camera/CameraComponent.h"
#include "Math/EngineMath.h"
#include "Engine/World.h"
#include "FontRenderer.h"
#include <algorithm>

class FUUIDBillboardRenderer
{
public:
	FUUIDBillboardRenderer() = default;
	~FUUIDBillboardRenderer() = default;
	void Init(UWorld* World);
	void SetUUIDTextItemList(UCameraComponent* Camera);
	const TArray<FWorldTextItem>& GetUUIDTextItemList() { return UUIDTextItemList; };
private:
	TArray<FWorldTextItem> UUIDTextItemList;
	UCameraComponent* Camera;
	UWorld* World;
};