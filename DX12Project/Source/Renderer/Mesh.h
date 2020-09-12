#pragma once
#include <DirectXCollision.h>
#include <d3d12.h>
#include "D3D12Resource.h"
#include "MathHelper.h"
#include "Component.h"
#include "GeometryGenerator.h"

using namespace DirectX;

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexC;
};

struct Geometry
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;

	// Bounding box of the geometry defined by this submesh. 
	DirectX::BoundingBox Bounds;
};