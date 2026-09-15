#include "EnginePCH.h"
#include "Types.h"
#include "FName.h"
#include "FNamePool.h"
#include <cctype>
#include <iostream>

FName::FName(const char* pStr)
{
	// NULL 포인터 예외처리
	if (!pStr) { return; }

	// 허용되지 않는 문자 포함 여부 검사
	const char InvalidCharacters[] = { '\\', '\n', '\r', '\t' };
	for (const char& Invalid : InvalidCharacters)
	{
		if (std::strchr(pStr, Invalid))
		{
			return;
		}
	}

	// 문자열 소문자로 변환
	std::string LowerStr(pStr);

	for (char& C : LowerStr) 
	{
		C = static_cast<char>(std::tolower(static_cast<unsigned char>(C)));
	}

	// 맨뒤 Number 있는지 검사 후 나누기
	size_t LastSeparator = LowerStr.find_last_of("_");
	if (LastSeparator != std::string::npos)
	{
		FString Remaining = LowerStr.substr(LastSeparator + 1, LowerStr.length());
		int32 Num = ExtractNumber(Remaining);

		if (Num >= 0)
		{
			Number = Num;
			LowerStr = LowerStr.substr(0, LastSeparator);
		}
	}

	// FNamePool FindOrAdd 호출
	ComparisonIndex = FNamePool::Get().FindOrAdd(LowerStr);
}

FName::FName(FString str) : FName(str.c_str())
{
}

int32 FName::Compare(const FName& Other) const
{
	// 인덱스 같을 시 Number 차이 반환
	if (ComparisonIndex == Other.ComparisonIndex)
	{
		return Number - Other.Number;
	}

	// 완전 다른 문자열 시 직접 비교
	const FNamePool& Pool = FNamePool::Get();
	FString MyString = Pool.GetString(ComparisonIndex);
	FString OtherString = Pool.GetString(Other.ComparisonIndex);

	return MyString.compare(OtherString);
}

bool FName::operator==(const FName& Other) const
{
	return ComparisonIndex == Other.ComparisonIndex && Number == Other.Number;
}

bool FName::IsNone()
{
	// 인덱스가 0이거나 Number가 -1 이면 NoneName
	return ComparisonIndex == 0 && Number == -1;
}

FString FName::GetString()
{
	//return FNamePool::Get().GetString(ComparisonIndex);

	FString Result = FNamePool::Get().GetString(ComparisonIndex);

	if (Number >= 0)
	{
		Result += FString("_");
		Result += std::to_string(Number);
	}

	return Result;
}

int32 FName::ExtractNumber(FString From)
{
	try {
		int32 Num = std::stoi(From);
		return Num;
	}
	catch (const std::invalid_argument& e) {
		return -1;
	}
	catch (const std::out_of_range& e) {
		return -1;
	}
}