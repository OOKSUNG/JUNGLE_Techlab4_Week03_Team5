#pragma once

#include "Math/Vector.h"

struct FBoxBounds
{
	FVector Origin;
	FVector BoxExtent;

	inline const FVector GetMin() const { return Origin - BoxExtent; }
	inline const FVector GetMax() const { return Origin + BoxExtent; }
};