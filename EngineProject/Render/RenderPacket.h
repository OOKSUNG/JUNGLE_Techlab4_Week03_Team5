#pragma once
#include "Math/Transform.h"
#include "Mesh.h"
#include "Shader.h"

class UPrimitiveComponent;

struct FRenderPacket {
	FMatrix model;
	FMesh* mesh;
	FShader* shader;
	bool bIsVisible;
	UPrimitiveComponent* Owner = nullptr; // WireFrame 모드 시, 선택 객체 판별용
	
	//subUV용 파라미터
	ID3D11ShaderResourceView* TextureSRV = nullptr;
	ID3D11SamplerState* SamplerState = nullptr;
	FVector2 UVOffset = FVector2(0.0f, 0.0f);
	FVector2 UVScale = FVector2(1.0f, 1.0f);
};