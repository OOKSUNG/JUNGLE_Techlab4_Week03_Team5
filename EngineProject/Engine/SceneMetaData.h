#pragma once

#include "Core/Types.h"
#include "GameFramework/Actor.h"

struct FSceneMetaData
{
	int Version;
	uint32 NextUUID;
	TArray<AActor*> Actors;
};