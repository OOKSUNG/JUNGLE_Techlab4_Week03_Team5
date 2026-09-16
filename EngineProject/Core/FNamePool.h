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
	FString GetString(int32 Index) const;

private:
	FNamePool();
	~FNamePool();

	void Rehash();

	// Comparison
	int32* ComparisonHashTable;
	TArray<FString> ComparisonStringList;
	int32 ComparisonTableSize = 8192;

	int32* HashTable;
	TArray<FString> StringList;
	int32 TableSize = 8192;
};