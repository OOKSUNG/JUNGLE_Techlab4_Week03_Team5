#include "EnginePCH.h"
#include "Buffer.h"

FIndexBuffer::FIndexBuffer(ID3D11Device* Device, const uint32* Indices, uint32 Count)
{
	IndexCount = Count;

	// Create a index buffer
	D3D11_BUFFER_DESC Desc = {};
	Desc.ByteWidth = sizeof(uint32) * Count;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA Data;
	Data.pSysMem = Indices;

	HRESULT hr = Device->CreateBuffer(&Desc, &Data, Buffer.GetAddressOf());
}

FVertexBuffer::FVertexBuffer(ID3D11Device* Device, const void* Vertices, uint32 TotalSize, uint32 InStride)
{
	Stride = InStride;

	// Create a index buffer
	D3D11_BUFFER_DESC Desc = {};
	Desc.ByteWidth = TotalSize;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA Data;
	Data.pSysMem = Vertices;

	HRESULT hr = Device->CreateBuffer(&Desc, &Data, Buffer.GetAddressOf());
}

FConstantBuffer::FConstantBuffer(ID3D11Device* Device, uint32 Size)
{
	D3D11_BUFFER_DESC Desc = {};
	Desc.ByteWidth = Size;
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = Device->CreateBuffer(&Desc, nullptr, Buffer.GetAddressOf());
}

void FConstantBuffer::UpdateData(const void* InData)
{
	
}

FDynamicVertexBuffer::FDynamicVertexBuffer(ID3D11Device* Device, uint32 InCapacity, uint32 InStride)
{
	Stride = InStride;
	Capacity = InCapacity;

	// Create a DynamicVertex buffer
	D3D11_BUFFER_DESC Desc = {};
	Desc.ByteWidth = InStride * InCapacity ; // sizeof(Vertex) * Capacity
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = Device->CreateBuffer(&Desc, nullptr, Buffer.GetAddressOf());

	assert(SUCCEEDED(hr));

}

bool FDynamicVertexBuffer::Update( ID3D11DeviceContext* Context, const void* Data, uint32 DataSize)
{
	if (DataSize > Stride * Capacity)
	{
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE Mapped = {};

	HRESULT hr = Context->Map( Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped );

	if (FAILED(hr))
	{
		return false;
	}

	memcpy( Mapped.pData, Data, DataSize );

	Context->Unmap(Buffer.Get(), 0);

	return true;
}

FDynamicIndexBuffer::FDynamicIndexBuffer(ID3D11Device* Device, uint32 InCapacity)
{
	Capacity = InCapacity;

	// Create a DynamicIndex buffer
	D3D11_BUFFER_DESC Desc = {};
	Desc.ByteWidth = sizeof(uint32) * InCapacity; // sizeof(Vertex) * Capacity
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = Device->CreateBuffer(&Desc, nullptr, Buffer.GetAddressOf());

	assert(SUCCEEDED(hr));

}

bool FDynamicIndexBuffer::Update(ID3D11DeviceContext* Context, const uint32* Indices, uint32 IndexCount)
{
	// 현재 넣으려는 인덱스 개수가 버퍼 용량을 초과했는지 확인
	if (IndexCount > Capacity)
	{
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE Mapped = {};

	HRESULT hr = Context->Map( Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);

	if (FAILED(hr))
	{
		return false;
	}

	memcpy( Mapped.pData, Indices, sizeof(uint32) * IndexCount );
	Context->Unmap(Buffer.Get(), 0);

	return true;
}