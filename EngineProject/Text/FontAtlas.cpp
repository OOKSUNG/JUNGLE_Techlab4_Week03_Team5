#include "EnginePCH.h"
#include "FontAtlas.h"
#include "Render/Renderer.h"
#include <algorithm>

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

    if (err != 0) return false;

    if (FT_New_Face(FTLibrary, PathBuf, 0, &FTFace) != 0) return false;

    FT_Set_Pixel_Sizes(FTFace, 0, FontPixelSize);

    // FTFace를 msdfgen이 그대로 재사용 (폰트 파일 동일)
    MsdfFont = msdfgen::adoptFreetypeFont(FTFace);
    if (!MsdfFont) assert(false, "MsdfFont Error");

    if (CreatePage() < 0) return false;

    return true;
}

int FDynamicFontAtlas::CreatePage()
{
    if (static_cast<int>(Pages.size()) >= MaxPages)
    {
        if (!bPageLimitWarned)
        {
            LOG(Renderer, Error, "Font atlas reached MaxPages({}) limit! New glyphs will render blank.", MaxPages);
            bPageLimitWarned = true;
        }
        return -1;
    } 

    FAtlasPage Page;
    // CPU AtlasBuffer (RGBA - msdf 3채널 + 패딩)
    Page.Buffer.resize(AtlasSize * AtlasSize * BytesPerPixel, 0);

    // GPU 텍스처
    D3D11_TEXTURE2D_DESC Desc = {};
    Desc.Width = AtlasSize;
    Desc.Height = AtlasSize;
    Desc.MipLevels = 1;
    Desc.ArraySize = 1;
    Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    Desc.SampleDesc.Count = 1;
    Desc.Usage = D3D11_USAGE_DEFAULT;
    Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    Desc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = Page.Buffer.data();
    InitData.SysMemPitch = AtlasSize * BytesPerPixel;

    if (FAILED(Device->CreateTexture2D(&Desc, &InitData, &Page.Texture))) return -1;
    if (FAILED(Device->CreateShaderResourceView(Page.Texture, nullptr, &Page.SRV))) return -1;

    // Shelf Packing 상태 초기화
    Pages.push_back(Page);

    // Page Limit(6) 알려주기
    LOG(Renderer, Warning, "Font atlas created page {} / {}.", Pages.size(), MaxPages);

    return static_cast<int>(Pages.size()) - 1;

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
    static constexpr int GlyphPadding = 1 ; // 인접 Glyph 블리딩 방지용 패딩

    // 1. 윤곽선(Shape) 추출
    msdfgen::Shape Shape;
    double Advance = 0.0;
    bool bLoaded = msdfgen::loadGlyph(
        Shape, MsdfFont, static_cast<msdfgen::unicode_t>(Codepoint),
        msdfgen::FONT_SCALING_EM_NORMALIZED, &Advance);
    
    if (!bLoaded)
    {
        LOG(Renderer, Error, "Failed to load glyph shape for codepoint {}!", Codepoint);
        
        FGlyphInfo FallbackInfo{};
        auto Result = GlyphCache.emplace(Codepoint, FallbackInfo);
        return Result.first->second;
    }
    
    Shape.normalize();
    Shape.orientContours();
    msdfgen::edgeColoringSimple(Shape, 3.0);
    
    // 2. 바운딩 박스 -> 픽셀 크기 계산
    double ShapeLeft = 0, ShapeBottom = 0, ShapeRight = 0, ShapeTop = 0;
    Shape.bound(ShapeLeft, ShapeBottom, ShapeRight, ShapeTop);
    
    int GlyphW = static_cast<int>(std::ceil((ShapeRight - ShapeLeft) * FontPixelSize)) + static_cast<int>(PxRange) * 2;
    int GlyphH = static_cast<int>(std::ceil((ShapeTop - ShapeBottom) * FontPixelSize)) + static_cast<int>(PxRange) * 2;
    
    // 그릴 윤곽선이 없을 경우
    if (GlyphW <= 0 || GlyphH <= 0)
    {
        LOG(Renderer, Error, "There is no shape in {}!", Codepoint);
        FGlyphInfo Info{};
        Info.Advance = static_cast<float>(Advance * FontPixelSize);
        auto Result = GlyphCache.emplace(Codepoint, Info);
        return Result.first->second;
    }

    // Shelf Packing
    int PageIndex = static_cast<int>(Pages.size()) - 1;
    FAtlasPage* Page = &Pages[PageIndex];

    // 현재 선반이 꽉 찼을 경우
    if (Page->CursorX + GlyphW + GlyphPadding > AtlasSize)
    {
        // LOG(Renderer, Error, "Font Atlas is full! Codepoint {} will render in next shelf.", Codepoint);
        Page->CursorX = 0;
        Page->CursorY += Page->CurrentShelfHeight;
        Page->CurrentShelfHeight = 0;
    }
    // 전체 선반이 꽉 찼을 경우 (Atlas가 꽉 찼을 경우)
    if (Page->CursorY + GlyphH + GlyphPadding > AtlasSize)
    {   
        // glyph 자체가 페이지보다 큰 경우 (진짜 실패)
        if (GlyphW > AtlasSize | GlyphH > AtlasSize)
        {
            LOG(Renderer, Error, "Glyph {} is larger than atlas page!", Codepoint);
            
            FGlyphInfo FallbackInfo{};
            FallbackInfo.Advance = static_cast<float>(Advance * FontPixelSize);
            auto Result = GlyphCache.emplace(Codepoint, FallbackInfo);
            return Result.first->second;
        }

        PageIndex = CreatePage();
        
        if (PageIndex <0)
        {
            // MaxPage 도달 - CreatePage에서 경고 로그 남김
            LOG(Renderer, Error, "Failed to allocate new atlas page for codepoint {}!", Codepoint);
            FGlyphInfo FallbackInfo{};
            FallbackInfo.Advance = static_cast<float>(Advance * FontPixelSize);
            auto Result = GlyphCache.emplace(Codepoint, FallbackInfo);
            return Result.first->second;
        }
        
        // 다음 Atlas 페이지 생성
        Page = &Pages[PageIndex];
        LOG(Renderer, Warning, "Atlas page full, created page {} for codepoint {}.", PageIndex, Codepoint);
    }
    
    int DestX = Page->CursorX;
    int DestY = Page->CursorY;
    
    // 3. MSDF 생성
    msdfgen::Bitmap<float, 3> Msdf(GlyphW, GlyphH);
    
    msdfgen::Projection Proj(
        msdfgen::Vector2(FontPixelSize, FontPixelSize),
        msdfgen::Vector2(-ShapeLeft + PxRange / FontPixelSize, -ShapeBottom + PxRange / FontPixelSize));
        msdfgen::Range Range(PxRange / FontPixelSize);
        
    msdfgen::generateMSDF(Msdf, Shape, msdfgen::SDFTransformation(Proj, Range));
    
    // 4. CPU 버퍼에 글리프 픽셀 복사 (msdfgen은 y가 위로 갈수록 증가)
    for (int y = 0; y < GlyphH; y++)
    {
        for (int x = 0; x < GlyphW; x++)
        {
            const float* Px = Msdf(x, GlyphH -1 - y);
            int DstIndex = ((DestY + y) * AtlasSize + (DestX + x)) * BytesPerPixel;
            Page->Buffer[DstIndex + 0] = static_cast<uint8>(std::clamp(Px[0], 0.0f, 1.0f) * 255.0f);
            Page->Buffer[DstIndex + 1] = static_cast<uint8>(std::clamp(Px[1], 0.0f, 1.0f) * 255.0f);
            Page->Buffer[DstIndex + 2] = static_cast<uint8>(std::clamp(Px[2], 0.0f, 1.0f) * 255.0f);
            Page->Buffer[DstIndex + 3] = 255;
        }
    }
    
    // 선반 커서 갱신
    Page->CursorX += GlyphW + GlyphPadding;
    Page->CurrentShelfHeight = std::max(Page->CurrentShelfHeight, GlyphH + GlyphPadding);
    
    // GPU 텍스처에 업로드
    UploadAtlasToGPU(Context, PageIndex, DestX, DestY, GlyphW, GlyphH);
    
    // UV 계산 및 캐시에 등록
    FGlyphInfo Info;
    Info.PageIndex = PageIndex;
    Info.U = static_cast<float>(DestX) / AtlasSize;
    Info.V = static_cast<float>(DestY) / AtlasSize;
    Info.Width = static_cast<float>(GlyphW) / AtlasSize;
    Info.Height = static_cast<float>(GlyphH) / AtlasSize;
    Info.BitmapWidth = GlyphW;
    Info.BitmapHeight = GlyphH;
    Info.BearingX = static_cast<int>(std::floor(ShapeLeft * FontPixelSize)) - static_cast<int>(PxRange);
    Info.BearingY = static_cast<int>(std::ceil(ShapeTop * FontPixelSize)) + static_cast<int>(PxRange);
    Info.Advance = static_cast<float>(Advance * FontPixelSize);
    
    
    // 삽입과 반환을 동시에
    auto Result = GlyphCache.emplace(Codepoint, Info);
    return Result.first->second;
    
}

void FDynamicFontAtlas::UploadAtlasToGPU(ID3D11DeviceContext* Context, int PageIndex, int DirtyX, int DirtyY, int DirtyW, int DirtyH)
{
    FAtlasPage& Page = Pages[PageIndex];
    D3D11_BOX Box;
    Box.left = DirtyX;
    Box.top = DirtyY;
    Box.front = 0;
    Box.right = DirtyX + DirtyW;
    Box.bottom = DirtyY + DirtyH;
    Box.back = 1;

    const uint8* SrcData = &Page.Buffer[(DirtyY * AtlasSize + DirtyX) * BytesPerPixel];
    Context->UpdateSubresource(Page.Texture, 0, &Box, SrcData, AtlasSize * BytesPerPixel, 0);
}


bool FDynamicFontAtlas::SaveDebugBMP(FRenderer* Renderer, const char* FilePath, int PageIndex) const
{
    ID3D11Device* Device = Renderer->GetDevice();
    ID3D11DeviceContext* Context = Renderer->GetDeviceContext();
    int Size = AtlasSize;

    // 1. CPU에서 읽을 수 있는 STAGING 텍스처 생성 (아틀라스와 동일한 포맷이어야 CopyResource 가능)
    D3D11_TEXTURE2D_DESC Desc = {};
    Desc.Width = Size;
    Desc.Height = Size;
    Desc.MipLevels = 1;
    Desc.ArraySize = 1;
    Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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
    Context->CopyResource(StagingTex, Pages[PageIndex].Texture);

    // 3. Map해서 CPU로 픽셀 읽기
    D3D11_MAPPED_SUBRESOURCE Mapped;
    if (FAILED(Context->Map(StagingTex, 0, D3D11_MAP_READ, 0, &Mapped)))
    {
        std::cout << "Map failed" << std::endl;
        StagingTex->Release();
        return false;
    }

    int RowSize = (Size * 3 + 3) & ~3; // 24bit BMP는 행마다 4바이트 정렬 필요
    TArray<uint8> PixelData(RowSize * Size);

    const uint8* Src = static_cast<const uint8*>(Mapped.pData);
    for (int y = 0; y < Size; y++)
    {
        // BMP는 아래->위 순서로 저장하므로 y를 뒤집어서 복사
        uint8* DstRow = &PixelData[(Size - 1 - y) * RowSize];
        const uint8* SrcRow = Src + y * Mapped.RowPitch;

        for (int x = 0; x < Size; x++)
        {
            // RGBA(MSDF R/G/B + 미사용 A) -> BMP는 BGR 순서로 저장
            DstRow[x * 3 + 0] = SrcRow[x * 4 + 2]; // B
            DstRow[x * 3 + 1] = SrcRow[x * 4 + 1]; // G
            DstRow[x * 3 + 2] = SrcRow[x * 4 + 0]; // R
        }
    }

    Context->Unmap(StagingTex, 0);
    StagingTex->Release();

    // 4. 24bit 트루컬러 BMP로 저장 (팔레트 없음)
    FBmpFileHeader FileHeader;
    FBmpInfoHeader InfoHeader;
    InfoHeader.Width = Size;
    InfoHeader.Height = Size;
    InfoHeader.BitCount = 24;
    InfoHeader.ClrUsed = 0;
    InfoHeader.ClrImportant = 0;
    InfoHeader.SizeImage = static_cast<uint32>(PixelData.size());

    FileHeader.OffBits = sizeof(FBmpFileHeader) + sizeof(FBmpInfoHeader);
    FileHeader.Size = FileHeader.OffBits + static_cast<uint32>(PixelData.size());

    std::ofstream File(FilePath, std::ios::binary);
    File.write(reinterpret_cast<const char*>(&FileHeader), sizeof(FileHeader));
    File.write(reinterpret_cast<const char*>(&InfoHeader), sizeof(InfoHeader));
    File.write(reinterpret_cast<const char*>(PixelData.data()), PixelData.size());

    // std::cout << "Atlas dumped to " << FilePath << std::endl;

    return true;
}

