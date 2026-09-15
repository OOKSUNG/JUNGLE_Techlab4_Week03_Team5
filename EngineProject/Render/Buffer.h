#pragma once

#include <d3d11.h>
#include <wrl.h>

using namespace Microsoft::WRL;

class FIndexBufferBase
{
public:
	virtual ~FIndexBufferBase() = default;
	virtual ID3D11Buffer* GetBuffer() const = 0;
};

class FVertexBufferBase
{
public:
	virtual ~FVertexBufferBase() = default;

	virtual ID3D11Buffer* GetBuffer() const = 0;
	virtual uint32 GetStride() const = 0;
};

class FIndexBuffer : public FIndexBufferBase
{
public:
	FIndexBuffer(ID3D11Device* Device, const uint32* Indices, uint32 IndexCount);
	// ~FIndexBuffer() = default;

	inline uint32 GetIndexCount() const { return IndexCount; }
	inline ID3D11Buffer* GetBuffer() const override { return Buffer.Get(); }

private:
	uint32 IndexCount;
	ComPtr<ID3D11Buffer> Buffer;

};

class FVertexBuffer : public FVertexBufferBase
{
public:
	FVertexBuffer(ID3D11Device* Device, const void* Vertices, uint32 TotalSize, uint32 InStride);

	inline uint32 GetStride() const override { return Stride; }
	inline ID3D11Buffer* GetBuffer() const override  { return Buffer.Get(); }

private:
	uint32 Stride = 0;
	ComPtr<ID3D11Buffer> Buffer;

};

class FConstantBuffer
{
public:
	FConstantBuffer(ID3D11Device* Device, uint32 Size);
	~FConstantBuffer() = default;

	void UpdateData(const void* InData);
	inline ID3D11Buffer* GetBuffer() const { return Buffer.Get(); }

private:
	ComPtr<ID3D11Buffer> Buffer;
	void* DataPtr;
	uint32 Size;

};

class FDynamicVertexBuffer : public FVertexBufferBase
{
public:
	FDynamicVertexBuffer(ID3D11Device* Device, uint32 InCapacity, uint32 InStride);
	// ~FDynamicVertexBuffer() = default;

	bool Update(ID3D11DeviceContext* Context, const void* Data, uint32 DataSize);

	inline uint32 GetStride() const override { return Stride; }
	inline ID3D11Buffer* GetBuffer() const override { return Buffer.Get(); }

private:
	uint32 Stride = 0;
	uint32 Capacity = 0;

	ComPtr<ID3D11Buffer> Buffer;
};

class FDynamicIndexBuffer : public FIndexBufferBase
{
public:
	FDynamicIndexBuffer( ID3D11Device* Device, uint32 InCapacity);

	bool Update( ID3D11DeviceContext* Context, const uint32* Indices, uint32 IndexCount);

	ID3D11Buffer* GetBuffer() const override { return Buffer.Get(); }

	uint32 GetCapacity() const  { return Capacity; }

private:
	uint32 Capacity = 0;
	ComPtr<ID3D11Buffer> Buffer;
};