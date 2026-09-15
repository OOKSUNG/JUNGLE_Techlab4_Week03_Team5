#pragma once
#include <Windows.h>
#include <d3d11.h>
#include "Render/Renderer.h"
#include "FontManager.h"
#include "Camera/CameraComponent.h"

struct FWorldTextItem
{
	FString Text;
	FVector WorldStartPos; // 빌보드가 처음 시작하는 좌 중단 좌표값
	FVector CamRight;
	FVector CamUp;
	float DistSqr;
	float Scale = 1.0f;
};

struct FFontVertex // 정점 버퍼
{
	FVector Position;
	FVector2 UV;
};

struct FFontData // 상수 버퍼
{
	FMatrix VP;
	FVector4 color;
};

class FFontRenderer
{
public:
	FFontRenderer() = default;
	~FFontRenderer() = default;

	bool Init(FRenderer* InRenderer);
	void Begin();
	void End();
	void Shutdown();
	void RenderBatchTexts(TArray<FWorldTextItem> TextItemArray, UCameraComponent* Camera);
private:
	FRenderer* Renderer;
	float BaseFontSize;
	FFontTexture* FontTexture;

	TSharedPtr<FShader> Shader;
	TSharedPtr<FConstantBuffer> CB;
	TSharedPtr<FIndexBuffer> StaticIB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> DynamicVB;
	Microsoft::WRL::ComPtr<ID3D11BlendState> AlphaBlendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthReadOnlyState;

	const UINT MaxTextCount = 2048;
};