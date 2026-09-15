#pragma once
#include <vector>
#include <cstdint>
#include <string>

// UTF-8 문자열 -> 유니코드 코드포인트 배열
TArray<uint32> DecodeUTF8(const FString& Text);
