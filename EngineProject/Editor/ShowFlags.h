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

	/**
	* @brief ShowFlag에서 특정 Flag를 On/Off로 설정합니다.
	* @param Flag 설정하고자 하는 Flag의 열거형 값
	* @param bValue 활성화 여부 (true: On, false: Off)
	*/
	void Set(EShowFlagBits Flag, bool bValue)
	{
		if (bValue)
			Flags |= static_cast<uint64>(Flag);
		else
			Flags &= ~static_cast<uint64>(Flag);
	}
	/**
	* @brief ShowFlag에서 특정 Flag의 상태를 토글(반전)합니다.
	* @param Flag 반전하고자 하는 Flag의 열거형 값
	*/
	void Toggle(EShowFlagBits Flag)
	{
		Flags ^= static_cast<uint64>(Flag);
	}
	/**
	* @brief 특정 Flag가 활성화되어 있는지 확인합니다.
	* @param Flag 확인할 Flag의 열거형 값
	* @return 해당 Flag가 켜져 있으면 true, 꺼져 있으면 false
	*/
	bool IsSet(EShowFlagBits Flag)
	{
		return ((Flags & static_cast<uint64>(Flag)) != 0);
	}
	/**
	* @brief 씬 시작 시 전체 플래그의 값을 기본값(Default)으로 재설정합니다.
	*/
	void SetDefault()
	{
		Flags = static_cast<uint64>(EShowFlagBits::Default);
	}
	/**
	* @brief Flags를 세팅된 프리셋으로 설정합니다
	* @param FlagPreset 설정할 프리셋
	*/
	void SetFlagPreset(EShowFlagBits FlagPreset)
	{
		Flags = static_cast<uint64>(FlagPreset);
	}
};