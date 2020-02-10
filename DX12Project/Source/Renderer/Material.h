#pragma once
#include "Object.h"

using namespace DirectX;

struct MaterialConstants
{
	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = 0.25f;

	// Used in texture mapping.
	DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
};

// Simple struct to represent a material for our demos.  A production 3D engine
// would likely create a class hierarchy of Materials.
struct MaterialData
{
	// Unique material name for lookup.
	std::string Name;

	// Index into constant buffer corresponding to this material.
	int MatCBIndex = -1;

	// Index into SRV heap for diffuse texture.
	int DiffuseSrvHeapIndex = -1;

	// Index into SRV heap for normal texture.
	int NormalSrvHeapIndex = -1;

	// Dirty flag indicating the material has changed and we need to update the constant buffer.
	// Because we have a material constant buffer for each FrameResource, we have to apply the
	// update to each FrameResource.  Thus, when we modify a material we should set 
	// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
	int NumFramesDirty = gNumFrameResources;

	// Material constant buffer data used for shading.
	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = .25f;

	DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
};

class Material : public Component
{
public:
	Material();
	virtual ~Material();

	int GetDiffuseSrvHeapIndex() { return Data->DiffuseSrvHeapIndex; }

private:
	MaterialData* Data = nullptr;
};

// 
// class PrimitiveObject : public Object
// {
// 	MaterialData* Mat = nullptr;
// 
// 	// Primitive topology.
// 	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
// 
// 	// DrawIndexedInstanced parameters.
// 	UINT IndexCount = 0;
// 	UINT StartIndexLocation = 0;
// 	int BaseVertexLocation = 0;
// };