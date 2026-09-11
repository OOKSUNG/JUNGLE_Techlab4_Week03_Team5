#pragma once
#include "PrimitiveComponent.h"

class UPlaneComponent : public UPrimitiveComponent
{
	DECLARE_CLASS(UPlaneComponent, UPrimitiveComponent)

	REFLECT_START(ClassName)
		REFLECT_END()

		UPlaneComponent() {};
	~UPlaneComponent() {};

public:
	virtual void BeginPlay() override;
private:

};

