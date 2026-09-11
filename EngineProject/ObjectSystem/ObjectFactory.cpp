#include "EnginePCH.h"
#include "ObjectFactory.h"
#include "Class.h"

UObject* FObjectFactory::ConstructObject(UClass* Class)
{
    if (!Class || !Class->Constructor)
        return nullptr;
    
    UObject* Result = Class->Constructor();

    LOG(Object, Info, "Create {}", Class->Name);
    LOG(Object, Info, "Total Allocation Bytes - {}", FEngineStatics::TotalAllocationBytes);
    LOG(Object, Info, "Total Allocation Count - {}", FEngineStatics::TotalAllocationCount);

    return Result;
}
