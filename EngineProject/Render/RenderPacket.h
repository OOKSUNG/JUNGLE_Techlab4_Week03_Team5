#pragma once
#include "Math/Transform.h"
#include "Mesh.h"
#include "Shader.h"

class UPrimitiveComponent;

struct FRenderPacket {
	FMatrix model;
	FMesh* mesh;
	FShader* shader;
	UPrimitiveComponent* Owner = nullptr; // WireFrame 모드 시, 선택 객체 판별용
};