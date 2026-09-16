#pragma once
#include "PrimitiveComponent.h"


class UTextComponent :public UPrimitiveComponent
{
    DECLARE_CLASS(UTextComponent, UPrimitiveComponent)
    
	REFLECT_START(ClassName)
    PROPERTY(Text)          // Text 내용
    PROPERTY(FontPath)      // Font 경로
    PROPERTY(FontPixelSize) // Font Size
    
	REFLECT_END()
    
	UTextComponent() {};
	~UTextComponent() {};
    
public:
    static constexpr float WorldScaleFactor = 0.03f;
	virtual void BeginPlay() override;
    virtual void UpdateBounds() override;

    void SetText(const FString& InText) { Text = InText; bLayoutDirty = true; }
    const FString& GetText() const { return Text; }
    
    void SetColor(const FVector4& InColor) { Color = InColor; }
    const FVector4& GetColor() const { return Color; }

    void SetFontPath(const FString& InFontPath) { FontPath = InFontPath; bLayoutDirty = true; }
    const FString& GetFontPath() const { return FontPath; }

    void SetFontPixelSize(int InFontPixelSize) { FontPixelSize = InFontPixelSize; bLayoutDirty = true; }
    int GetFontPixelSize() const { return FontPixelSize; } 

    // render하고 true -> false로 flag 변경
    bool ConsumeLayoutDirty() { bool b = bLayoutDirty; bLayoutDirty = false; return b; }
    void SetWorldBounds(const FVector& BoxMin, const FVector& BoxMax)
    {
        Bounds.Origin = (BoxMax + BoxMin) * 0.5f;
        Bounds.BoxExtent = (BoxMax - BoxMin) * 0.5f;
    }

    void SetLocalExtent(float InHalfWidth, float InHalfHeight)
    {
        LocalHalfWidth = InHalfWidth;
        LocalHalfHeight = InHalfHeight;
    }

private:
    FString Text = "Text";
    FVector4 Color = FVector4(1.0f, 1.0f, 1.0f, 1.0f); // Black
    FString FontPath = "ThirdParty\\Pretendard-Regular.otf";
    int FontPixelSize = 32;

    // 임시 cached bb size
    float LocalHalfWidth = 1.0f;
    float LocalHalfHeight = 0.5f;

    // 텍스트/폰트/크기가 바뀐 순간만 렌더하기 위해 flag 지정
    bool bLayoutDirty = true; 

};