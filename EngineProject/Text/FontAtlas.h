#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <d3d11.h>
#include <cassert>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <msdfgen.h>
#include <msdfgen-ext.h>

class FRenderer;

struct FGlyphInfo
{
    float U, V, Width, Height;      // Atlas 내 uv (0~1 정규화)
    int BitmapWidth, BitmapHeight;  // 픽셀 단위 (쿼드 계산용)
    int BearingX, BearingY;         // Baseline 기준 오프셋 (p는 baseline 아래)
    float Advance;                  // 다음 글자로 넘어갈 x 이동량
};

class FDynamicFontAtlas
{
public:
    bool Init(ID3D11Device* InDevice, const wchar_t* InFontPath, int InFontPixelSize, int InAtlasSize = 1024);
    const FGlyphInfo& GetOrCreateGlyph(uint32 Codepoint, ID3D11DeviceContext* Context);

    // 셰이더에 바인딩해서 실제로 화면에 그릴 때
    inline ID3D11ShaderResourceView* GetAtlasSRV() const { return AtlasSRV; }
    inline ID3D11Texture2D* GetAtlasTexture() const { return AtlasTexture; }
    inline int GetAtlasSize() const { return AtlasSize; }
    inline float GetPxRange() const { return PxRange; }

    // Debug용 함수 (아틀라스 텍스처 직접 보고 싶을 때)
    bool SaveDebugBMP(FRenderer* Renderer, const char* FilePath) const;

private:
    const FGlyphInfo& RasterizeAndPack(uint32 Codepoint, ID3D11DeviceContext* Context);
    void UploadAtlasToGPU(ID3D11DeviceContext* Context, int DirtyX, int DirtyY, int DirtyW, int DirtyH);

    FT_Library FTLibrary = nullptr;
    FT_Face FTFace = nullptr;
    msdfgen::FontHandle* MsdfFont = nullptr;    // FTFace를 msdfgen에서 사용하기 위한 handle

    ID3D11Device* Device = nullptr;
    ID3D11Texture2D* AtlasTexture = nullptr;
    ID3D11ShaderResourceView* AtlasSRV = nullptr;

    int AtlasSize = 1024;
    int FontPixelSize = 24;

    static constexpr int BytesPerPixel = 4;     // MSDF RGB 3채널 + 미사용 패딩 1채널
    static constexpr float PxRange = 4.0f;      // Distance Field 유효 범위

    TArray<uint8> AtlasBuffer;
    TMap<uint32, FGlyphInfo> GlyphCache;

    // Shelf Packing 상태
    int CursorX = 0, CursorY = 0, CurrentShelfHeight = 0;
};