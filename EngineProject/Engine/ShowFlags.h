#pragma once
#include "EnginePCH.h"

enum class EShowFlagBits : uint64
{
	None = 0ULL,
	Primitives = 1ULL << 0,
	Grid = 1ULL << 1,
	OutLine = 1ULL << 2,
	Gizmo = 1ULL <<3,
	//UUID = 1ULL << 3,
	//BoundingBox = 1ULL << 4,

	Default = static_cast<uint64>(Primitives) | static_cast<uint64>(Grid) | static_cast<uint64>(OutLine) | static_cast<uint64>(Gizmo),
	All = static_cast<uint64>(Primitives) | static_cast<uint64>(Grid) | static_cast<uint64>(OutLine) | static_cast<uint64>(Gizmo)
};

struct FEngineShowFlags
{
	uint64 Flags = static_cast<uint64>(EShowFlagBits::Default);

	// 설정
	void Set(EShowFlagBits Flag, bool bValue)
	{
		if (bValue)
			Flags |= static_cast<uint64>(Flag);
		else
			Flags &= ~static_cast<uint64>(Flag);
	}
	//토글
	void Toggle(EShowFlagBits Flag)
	{
		Flags ^= static_cast<uint64>(Flag);
	}
	// 확인 IsSet
	bool IsSet(EShowFlagBits Flag)
	{
		return ((Flags & static_cast<uint64>(Flag)) != 0);
	}
	// 신 리셋시 기본값 SetDefault
	void SetDefault()
	{
		Flags = static_cast<uint64>(EShowFlagBits::Default);
	}

};