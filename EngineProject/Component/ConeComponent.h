#pragma once
#include "PrimitiveComponent.h"
class UConeComponent : public UPrimitiveComponent
{
	DECLARE_CLASS(UConeComponent, UPrimitiveComponent)

	REFLECT_START(ClassName)
		REFLECT_END()

		UConeComponent() {};
	~UConeComponent() {};

public:
	virtual void BeginPlay() override;

private:



};

