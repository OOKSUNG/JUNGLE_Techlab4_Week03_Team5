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

    void SetText(const FString& InText) { Text = InText; }
    const FString& GetText() const { return Text; }
    
    void SetColor(const FVector4& InColor) { Color = InColor; }
    const FVector4& GetColor() const { return Color; }

    void SetFontPath(const FString& InFontPath) { FontPath = InFontPath; }
    const FString& GetFontPath() const { return FontPath; }

    void SetFontPixelSize(int InFontPixelSize) { FontPixelSize = InFontPixelSize; }
    int GetFontPixelSize() const { return FontPixelSize; } 

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

};