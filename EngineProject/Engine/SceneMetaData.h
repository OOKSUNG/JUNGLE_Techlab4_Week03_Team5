#pragma once

#include "Core/Types.h"
#include "Core/Containers.h"
#include "Math/Transform.h"
#include "Core/EngineString.h"


// Text 내용, 폰트, 사이즈, 컬러 저장
struct FTextMetaData
{
	FString Text;
	FString FontPath;
	int FontPixelSize;
	FVector4 Color;
};

struct FParticleMetaData
{
	FString ParticlePath;
};

struct FSceneMetaData
{
	int Version;
	uint32 NextUUID;
	TArray<uint32> UUIDs;
	TMap<uint32, FTransform> Transforms;
	TMap<uint32, FString> Types;
	TMap<uint32, FTextMetaData> TextDatas;	// Text property
	TMap<uint32, FParticleMetaData> ParticleDatas;

	TMap<uint32, FString> Names;	
};