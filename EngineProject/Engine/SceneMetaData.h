#pragma once

#include "Core/Types.h"
#include "Core/Containers.h"
#include "Math/Transform.h"
#include "Core/EngineString.h"

struct FSceneMetaData
{
	int Version;
	uint32 NextUUID;
	TArray<uint32> UUIDs;
	TMap<uint32, FTransform> Transforms;
	TMap<uint32, FString> Types;
};