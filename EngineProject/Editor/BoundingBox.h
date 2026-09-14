#pragma once

#include "../Component/PrimitiveComponent.h"

class FBoundingBox
{
public:
	void SetTarget(UPrimitiveComponent* InTarget) { Target = InTarget; }
	UPrimitiveComponent* GetTarget() const { return Target; }
private:
	UPrimitiveComponent* Target = nullptr;
};