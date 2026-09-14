#pragma once
//
//#include <d3d11.h>
//#include "Render/Renderer.h"
//#include "LineRenderer.h"
//
//struct GridData
//{
//	FMatrix invViewProj;
//	FVector CameraPos;
//	float padding;
//};
//
//class FGridRenderer
//{
//public:
//	~FGridRenderer();
//
//	bool Init(FRenderer* InRenderer);
//	//bool Init(TSharedPtr<FLineRenderer>  InLineRenderer);
//	void OnRender(const FMatrix& ViewProj, const FVector& CameraPos);
//	
//private:
//	FRenderer* Renderer;
//	TSharedPtr<FLineRenderer> LineRenderer;
//
//	TSharedPtr<FShader> GridShader;
//	FShader* AxisShader;
//
//	TSharedPtr<FVertexBuffer> VB;
//	TSharedPtr<FIndexBuffer> IB;
//	TSharedPtr<FConstantBuffer> CB;
//
//	Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterizerState;
//};