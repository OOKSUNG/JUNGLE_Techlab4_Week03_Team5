#include "EnginePCH.h"
#include "StringUtils.h"

// UTF-8 -> 유니코드
TArray<uint32> DecodeUTF8(const FString& Text)
{
    TArray<uint32> Codepoints;
    size_t i = 0;
    while (i < Text.size())
    {
        uint8 Byte0 = static_cast<uint8>(Text[i]);
        uint32 Codepoint = 0;
        int ExtraBytes = 0;

        if      ((Byte0 & 0x80) == 0x00)     { Codepoint = Byte0;        ExtraBytes = 0; }     // 상위 1바이트 (ASCII)
        else if ((Byte0 & 0xE0) == 0xC0)     { Codepoint = Byte0 & 0x1F; ExtraBytes = 1; }     // 상위 2바이트
        else if ((Byte0 & 0xF0) == 0xE0)     { Codepoint = Byte0 & 0x0F; ExtraBytes = 2; }     // 상위 3바이트 (한글)
        else if ((Byte0 & 0xF8) == 0xF0)     { Codepoint = Byte0 & 0x07; ExtraBytes = 3; }     // 상위 4바이트
        else { i++; continue; }

        i++;
        for (int b=0; b<ExtraBytes && i < Text.size(); b++, i++)
        {
            // 6자리 비워내고 새로 넣기
            Codepoint = (Codepoint << 6) | (static_cast<uint8_t>(Text[i]) & 0x3F);
        }

        Codepoints.push_back(Codepoint);
    }
    return Codepoints;
}
