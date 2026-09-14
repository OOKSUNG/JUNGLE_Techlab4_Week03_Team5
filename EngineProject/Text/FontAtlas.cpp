#include "EnginePCH.h"
#include "FontAtlas.h"
#include "Render/Renderer.h"

#pragma pack(push, 1)
struct FBmpFileHeader
{
    uint16 Type = 0x4D42; // 'BM'
    uint32 Size;
    uint16 Reserved1 = 0;
    uint16 Reserved2 = 0;
    uint32 OffBits;
};

struct FBmpInfoHeader
{
    uint32 Size = 40;
    int32  Width;
    int32  Height;
    uint16 Planes = 1;
    uint16 BitCount = 8;
    uint32 Compression = 0;
    uint32 SizeImage = 0;
    int32  XPelsPerMeter = 0;
    int32  YPelsPerMeter = 0;
    uint32 ClrUsed = 256;
    uint32 ClrImportant = 256;
};
#pragma pack(pop)

bool FDynamicFontAtlas::Init(ID3D11Device* InDevice, const wchar_t* InFontPath, int InFontPixelSize, int InAtlasSize)
{
    Device = InDevice;
    AtlasSize = InAtlasSize;
    FontPixelSize = InFontPixelSize;

    if (FT_Init_FreeType(&FTLibrary) != 0) return false;

    size_t ConvertedChars = 0;
    char PathBuf[260];
    errno_t err = wcstombs_s(&ConvertedChars, PathBuf, sizeof(PathBuf), InFontPath, _TRUNCATE);

    if (err != 0)
    {
        // 변환 실패
        return false;
    }

    if (FT_New_Face(FTLibrary, PathBuf, 0, &FTFace) != 0) return false;

    FT_Set_Pixel_Sizes(FTFace, 0, FontPixelSize);

    // CPU AtlasBuffer (GrayScale)
    AtlasBuffer.resize(AtlasSize * AtlasSize, 0);

    // GPU 텍스처
    D3D11_TEXTURE2D_DESC Desc = {};
    Desc.Width = AtlasSize;
    Desc.Height = AtlasSize;
    Desc.MipLevels = 1;
    Desc.ArraySize = 1;
    Desc.Format = DXGI_FORMAT_R8_UNORM;
    Desc.SampleDesc.Count = 1;
    Desc.Usage = D3D11_USAGE_DYNAMIC;
    Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    if (FAILED(Device->CreateTexture2D(&Desc, nullptr, &AtlasTexture))) return false;
    if (FAILED(Device->CreateShaderResourceView(AtlasTexture, nullptr, &AtlasSRV))) return false;

    // Shelf Packing 상태 초기화
    CursorX = 0;
    CursorY = 0;
    CurrentShelfHeight = 0;

    return true;

}

// Codepoint에 대한 Glyph 정보 반환 (없으면 Rasterizing)
const FGlyphInfo& FDynamicFontAtlas::GetOrCreateGlyph(uint32 Codepoint, ID3D11DeviceContext* Context)
{
    auto It = GlyphCache.find(Codepoint);
    if (It != GlyphCache.end()) return It->second;
    return RasterizeAndPack(Codepoint, Context);
}

const FGlyphInfo& FDynamicFontAtlas::RasterizeAndPack(uint32 Codepoint, ID3D11DeviceContext* Context)
{
    // Rasterizing
    FT_Load_Char(FTFace, Codepoint, FT_LOAD_RENDER);
    FT_GlyphSlot Slot = FTFace->glyph;
    FT_Bitmap& Bitmap = Slot->bitmap;

    int GlyphW = Bitmap.width;
    int GlyphH = Bitmap.rows;

    // Shelf Packing

    // 현재 선반이 꽉 찼을 경우
    if (CursorX + GlyphW > AtlasSize)
    {
        CursorX = 0;
        CursorY += CurrentShelfHeight;
        CurrentShelfHeight = 0;
    }
    // 전체 선반이 꽉 찼을 경우
    if (CursorY + GlyphH > AtlasSize)
    {
        assert(false && "Font Atlas is full!");
    }

    int DestX = CursorX;
    int DestY = CursorY;

    // CPU 버퍼에 글리프 픽셀 복사
    for (int y = 0; y < GlyphH; y++)
        for (int x = 0; x < GlyphW; x++)
            AtlasBuffer[(DestY + y) * AtlasSize + (DestX + x)] = Bitmap.buffer[y * Bitmap.pitch + x];

    // 선반 커서 갱신
    CursorX += GlyphW;
    CurrentShelfHeight = max(CurrentShelfHeight, GlyphH);

    // GPU 텍스처에 업로드
    UploadAtlasToGPU(Context);

    // UV 계산 및 캐시에 등록
    FGlyphInfo Info;
    Info.U = static_cast<float>(DestX) / AtlasSize;
    Info.V = static_cast<float>(DestY) / AtlasSize;
    Info.Width = static_cast<float>(GlyphW) / AtlasSize;
    Info.Height = static_cast<float>(GlyphH) / AtlasSize;
    Info.BitmapWidth = GlyphW;
    Info.BitmapHeight = GlyphH;
    Info.BearingX = Slot->bitmap_left;
    Info.BearingY = Slot->bitmap_top;
    Info.Advance = Slot->advance.x >> 6;        // 소수부 날림

    // 삽입과 반환을 동시에
    auto Result = GlyphCache.emplace(Codepoint, Info);
    return Result.first->second;

}

void FDynamicFontAtlas::UploadAtlasToGPU(ID3D11DeviceContext* Context)
{
    // 추후 D3D11_MAP_WRITE_NO_OVERWRITE, UpdateSubresource의 Box 파라미터로
    // 변경된 영역만 갱신하는 최적화를 적용할 필요가 있음.

    D3D11_MAPPED_SUBRESOURCE Mapped;
    Context->Map(AtlasTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);
    uint8* Dst = static_cast<uint8*>(Mapped.pData);
    for (int Row = 0; Row < AtlasSize; Row++)
        memcpy(Dst + Row * Mapped.RowPitch, &AtlasBuffer[Row * AtlasSize], AtlasSize);
    Context->Unmap(AtlasTexture, 0);
}


bool FDynamicFontAtlas::SaveDebugBMP(FRenderer* Renderer, const char* FilePath) const
{
    ID3D11Device* Device = Renderer->GetDevice();
    ID3D11DeviceContext* Context = Renderer->GetDeviceContext();
    int Size = AtlasSize;

    // 1. CPU에서 읽을 수 있는 STAGING 텍스처 생성
    D3D11_TEXTURE2D_DESC Desc = {};
    Desc.Width = Size;
    Desc.Height = Size;
    Desc.MipLevels = 1;
    Desc.ArraySize = 1;
    Desc.Format = DXGI_FORMAT_R8_UNORM;
    Desc.SampleDesc.Count = 1;
    Desc.Usage = D3D11_USAGE_STAGING;
    Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    ID3D11Texture2D* StagingTex = nullptr;
    if (FAILED(Device->CreateTexture2D(&Desc, nullptr, &StagingTex)))
    {
        std::cout << "Staging texture creation failed" << std::endl;
        return false;
    }

    // 2. GPU 원본 아틀라스 -> STAGING 복사
    Context->CopyResource(StagingTex, AtlasTexture);

    // 3. Map해서 CPU로 픽셀 읽기
    D3D11_MAPPED_SUBRESOURCE Mapped;
    if (FAILED(Context->Map(StagingTex, 0, D3D11_MAP_READ, 0, &Mapped)))
    {
        std::cout << "Map failed" << std::endl;
        StagingTex->Release();
        return false;
    }

    int RowSize = (Size + 3) & ~3; // BMP는 행마다 4바이트 정렬 필요
    TArray<uint8> PixelData(RowSize * Size);

    const uint8* Src = static_cast<const uint8*>(Mapped.pData);
    for (int y = 0; y < Size; y++)
    {
        // BMP는 아래->위 순서로 저장하므로 y를 뒤집어서 복사
        memcpy(&PixelData[(Size - 1 - y) * RowSize], Src + y * Mapped.RowPitch, Size);
    }

    Context->Unmap(StagingTex, 0);
    StagingTex->Release();

    // 4. 8bit 그레이스케일 BMP로 저장
    FBmpFileHeader FileHeader;
    FBmpInfoHeader InfoHeader;
    InfoHeader.Width = Size;
    InfoHeader.Height = Size;
    InfoHeader.SizeImage = static_cast<uint32>(PixelData.size());

    uint32 PaletteSize = 256 * 4;
    FileHeader.OffBits = sizeof(FBmpFileHeader) + sizeof(FBmpInfoHeader) + PaletteSize;
    FileHeader.Size = FileHeader.OffBits + static_cast<uint32>(PixelData.size());

    std::ofstream File(FilePath, std::ios::binary);
    File.write(reinterpret_cast<const char*>(&FileHeader), sizeof(FileHeader));
    File.write(reinterpret_cast<const char*>(&InfoHeader), sizeof(InfoHeader));

    for (int i = 0; i < 256; i++)
    {
        uint8 Gray[4] = { (uint8)i, (uint8)i, (uint8)i, 0 };
        File.write(reinterpret_cast<const char*>(Gray), 4);
    }

    File.write(reinterpret_cast<const char*>(PixelData.data()), PixelData.size());

    std::cout << "Atlas dumped to " << FilePath << std::endl;


    return true;
}

