#include "Containers.h"
#include "EngineString.h"
#include "Types.h"
#include "FNamePool.h"
#include "ThirdParty/xxHash/xxhash.h"
#include <iostream>

FNamePool::FNamePool()
{
	HashTable = new int32[TableSize];
	std::fill(HashTable, HashTable + TableSize, -1);
	
	// 0번째는 None 문자열로 고정
	StringList.push_back("none");
	HashTable[XXH32("none", 4, 0) % TableSize] = 0;
}

FNamePool::~FNamePool()
{
	delete[] HashTable;
	StringList.clear();
}

FNamePool& FNamePool::Get()
{
	static FNamePool instance;
	return instance;
}

int32 FNamePool::FindOrAdd(const char* pStr)
{
	// 만약 용량이 60% 찼다면 재해싱
	if (StringList.size() >= TableSize * 0.6)
	{
		Rehash();
	}

	// 해시값 기반 슬롯 계산
	uint32 HashValue = XXH32(pStr, strlen(pStr), 0);
	uint32 Slot = HashValue % TableSize;

	// 해당 슬롯이 이미 차있다면
	while (HashTable[Slot] != -1)
	{
		int32 StringIndex = HashTable[Slot];
		FString Stored = StringList[StringIndex];

		// 같은 문자열이면 재사용
		if (Stored == pStr)
		{
			return StringIndex;
		}

		// 해시 충돌 발생
		else 
		{
			Slot = (Slot + 1) % TableSize;
		}
	}

	StringList.push_back(pStr);
	uint32 StringIndex = StringList.size() - 1;
	HashTable[Slot] = StringIndex;
	return StringIndex;
}

int32 FNamePool::FindOrAdd(FString str)
{
	return FindOrAdd(str.c_str());
}

void FNamePool::Rehash()
{
	// 크기 두 배로 설정
	TableSize *= 2;
	delete[] HashTable;
	HashTable = new int32[TableSize];
	std::fill(HashTable, HashTable + TableSize, -1);

	// 기존 문자열들 재배치
	for (int i = 0; i < StringList.size(); ++i)
	{
		FString String = StringList[i];
		uint32 HashValue = XXH32(String.c_str(), String.length(), 0);
		uint32 NewSlot = HashValue % TableSize;

		while (HashTable[NewSlot] != -1)
		{
			NewSlot = (NewSlot + 1) % TableSize;
		}

		HashTable[NewSlot] = i;
	}
}

FString FNamePool::GetString(int32 Index) const
{
	return StringList[Index];
}