#pragma once
#include "EngineString.h"
#include "Core/Containers.h"

struct FName
{
	FName(FString str);
	FName(const char* pStr);

	int32 Compare(const FName& Other) const;
	bool operator==(const FName&) const;

	int32 ComparisonIndex = 0;
	int32 DisplayIndex = 0;
	int32 Number = -1;

	FString GetString();
	bool IsNone();

private:
	int32 ExtractNumber(FString From);
};