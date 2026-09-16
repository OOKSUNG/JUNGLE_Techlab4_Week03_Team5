#pragma once
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

#include "RenderPacket.h"
#include "Render/Buffer.h"
#include "GeometryGenerator.h"
#include "Render/ViewModeState.h"

// #include "LineRenderer.h"

enum EShaderBindFlagBits : uint32
{
	None = 0,
	Vertex = 1,
	Geometry = 1 << 2,
	Domain = 1 << 3,
	Hull = 1 << 4,
	Pixel = 1 << 5,
	Compute = 1 << 6
};

class FRenderer
{
public:
	void BeginFrame();
	void EndFrame();

	bool Init(HWND hWindow);

	void CreateDeviceAndSwapChain(HWND hWindow);
	void CreateFrameBuffer();
	void CreateRasterizerState();
	void CreateDepthStencilBufferAndState();
	void CreateBlendState();
	void CreateConstantBuffer();
	void CreateDefaultShader();
	void BindMainRenderTarget();


	inline ID3D11Device* GetDevice() const { return Device.Get(); }
	ID3D11DeviceContext* GetDeviceContext();

	TSharedPtr<FShader> CreateShader(const wchar_t* FileName, D3D11_INPUT_ELEMENT_DESC* InLayoutDesc, size_t InLayoutSize);
	TSharedPtr<FShader> CreateShader(const wchar_t* FileName, std::vector<D3D11_INPUT_ELEMENT_DESC> Layouts);
	TSharedPtr<FMesh> CreateMesh(const FMeshData& InMeshData);
	TSharedPtr<FMesh> CreateMesh(TSharedPtr<FVertexBuffer> VertexBuffer, TSharedPtr<FIndexBuffer> IndexBuffer);

	TSharedPtr<FVertexBuffer> CreateVertexBuffer(const void* InVertices, uint32 InSize, uint32 Stride);
	TSharedPtr<FIndexBuffer> CreateIndexBuffer(const uint32* InIndices, uint32 IndexCount);
	TSharedPtr<FConstantBuffer> CreateConstantBuffer(uint32 BufferSize);

	TSharedPtr<FVertexBuffer> CreateDynamicVertexBuffer(const void* InVertices, uint32 InSize, uint32 Stride);
	TSharedPtr<FIndexBuffer> CreateDynamicIndexBuffer(const uint32* InIndices, uint32 IndexCount);

	void UpdateConstantBufferData(FConstantBuffer* Buffer, const void* Data, uint32 DataSize);

	void UpdateConstantBuffer(const FMatrix& MVP, bool bHighlightEdge, const FVector& EdgeColor);
	void BindVertexBuffer(FVertexBufferBase* VertexBuffer);
	void BindIndexBuffer(FIndexBufferBase* IndexBuffer);
	void BindConstantBuffer(uint32 Slot, FConstantBuffer* ConstantBuffer, EShaderBindFlagBits FlagBits = None);
	void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology);
	void SetDepthStencilEnabled(bool bEnabled);
	void BindShader(FShader* InShader);
	void BindMesh(FMesh* InMesh);

	void Draw(uint32 VertexCount);
	void DrawIndexed(uint32 IndexCount);

	// 윈도우 사이즈 변경
	void Resize(int32 Width, int32 Height);

	/*void Prepare();*/

	void RenderAll(TQueue<FRenderPacket>& InQueue, FMatrix VP, UPrimitiveComponent* SelectedTarget);
	void DrawPacket(const FRenderPacket& Packet, FMatrix VP, UPrimitiveComponent* SelectedTarget);
	void DrawTexturePacket(const FRenderPacket& Packet, FMatrix VP, UPrimitiveComponent* SelectedTarget);

	void Shutdown();

	// ViewMode Setter
	inline void SetViewMode(EViewModeIndex Mode) { ViewModeState->SetMode(Mode); };
	
	// ViewMode Getter
	inline EViewModeIndex GetViewMode() const { return ViewModeState->GetMode(); };

	// Shader Resource View Getter
	// inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetRenderTargetSRV() { return FrameBufferSRV; }

	// Depth Test ONLY
	inline ID3D11DepthStencilState* GetDepthTestOnlyState() const { return DepthTestOnlyState.Get(); } // Return ReadOnly(TestOnly)State 
	// AlphaBlendState
	inline ID3D11BlendState* GetFontAlphaBlendState() const { return FontAlphaBlendState.Get(); } // Return AlphaBlendState 
	inline ID3D11BlendState* GetParticleAlphaBlendState() const { return ParticleAlphaBlendState.Get(); } // Return AlphaBlendState 

private:
	// Camera 
	struct FConstants
	{
		FMatrix MVP; 			// 64 byte
		uint32 bHighlightEdge;	// 4  byte
		float EdgeColor[3]; 	// 12 byte
	};

	struct FSubUVConstantBufferData
	{
		FMatrix MVP;
		int32 bHighlightEdge;
		float EdgeColor[3];
		FVector2 UVOffset;
		FVector2 UVScale;
	};

	Microsoft::WRL::ComPtr<ID3D11Device> Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> FrameBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> FrameBufferRTV;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> DepthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> FrameBufferDSV;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthDisabledState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthTestOnlyState;		// Test ON, Wirte OFF (ReadOnly)

	Microsoft::WRL::ComPtr<ID3D11BlendState> FontAlphaBlendState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> ParticleAlphaBlendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterizerState;

	Microsoft::WRL::ComPtr <ID3D11Buffer> ConstantBuffer;
	D3D11_VIEWPORT ViewportInfo;

	// imgui 렌더링용
	// Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> FrameBufferSRV;

	uint32 Width;
	uint32 Height;

	TSharedPtr<FShader> DefaultShader;

	FLOAT ClearColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };

	TSharedPtr<FViewModeState> ViewModeState;

	//TUniquePtr<FLineRenderer> LineRenderer;
};
