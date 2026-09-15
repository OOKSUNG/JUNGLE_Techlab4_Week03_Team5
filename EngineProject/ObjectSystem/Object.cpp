#include "EnginePCH.h"
#include "Object.h"

#include "Core/EngineStatics.h"
#include "ObjectSystem/Class.h"
#

TArray<UObject*> GUObjectArray;

UObject::UObject() : UUID(FEngineStatics::GetUUID()), InternalIndex(GUObjectArray.size()), Name("Object_" + std::to_string(UUID))

{
	// UUID = FEngineStatics::GetUUID();
	// InternalIndex = GUObjectArray.size();
	GUObjectArray.push_back(this);
	LOG(Object, Info, "UUID : {}", UUID);
	// LOG(Object, Info, "FName : {}", Name.GetString());
}

UObject::UObject(FString InName) : UUID(FEngineStatics::GetUUID()), InternalIndex(GUObjectArray.size()), Name(InName + "_" + std::to_string(UUID))

{
	GUObjectArray.push_back(this);
	LOG(Object, Info, "UUID : {}", UUID);
	// Name = FName("Object");
}

UObject::UObject(bool bRegister) : Name("Object_" + std::to_string(UUID))
{
}

UObject::~UObject()
{
	GUObjectArray[InternalIndex] = nullptr;
}

UClass* UObject::StaticClass()
{
	static UClass c;
	static bool bIsInit = false;
	if (!bIsInit)
	{
		c.Name = "Object";
		c.Super = nullptr;
		c.Constructor = []() -> UObject*
			{
				return new UObject();
			};
		bIsInit = true;
	}
	return &c;
}

UClass* UObject::GetClass() const
{
	return StaticClass();
}

bool UObject::IsA(UClass* Class)
{
	return GetClass()->IsChildOf(Class);
}

