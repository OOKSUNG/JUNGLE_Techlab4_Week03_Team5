#pragma once

class FNamePool
{
public:
	static FNamePool& Get();

	FNamePool(const FNamePool&) = delete;
	FNamePool& operator=(const FNamePool&) = delete;
	FNamePool(FNamePool&&) = delete;
	FNamePool& operator=(FNamePool&&) = delete;

	int32 FindOrAddComparison(const char* pStr);
	int32 FindOrAddComparison(FString str);

	int32 FindOrAddDisplay(const char* pStr);
	int32 FindOrAddDisplay(FString str);

	FString GetString(int32 Index) const;

private:
	FNamePool();
	~FNamePool();

	void Rehash(int32*& HashTable, int32& TableSize, TArray<FString>& StringList);

	// Comparison
	int32* ComparisonHashTable;
	TArray<FString> ComparisonStringList;
	int32 ComparisonTableSize = 8192;

	// Display
	int32* DisplayHashTable;
	TArray<FString> DisplayStringList;
	int32 DisplayTableSize = 8192;
};