#pragma once

#include "Core/FBoxBounds.h"
#include "../Math/Transform.h"
#include "ActorComponent.h"

class USceneComponent : public UActorComponent
{
	DECLARE_CLASS(USceneComponent, UActorComponent)

	REFLECT_START(ClassName)
		PROPERTY(bVisible)
	REFLECT_END()
public:
	FTransform* GetTransform();
	FMatrix GetWorldMatrix() const;
	const FBoxBounds& GetBounds() const;
	void SetTransform(FTransform transform);
	virtual void UpdateBounds() {};
private:
	//FTransform transform;
	bool bVisible = true;
protected:
	// Protected로 이동
	FTransform transform;
	FBoxBounds Bounds;
};