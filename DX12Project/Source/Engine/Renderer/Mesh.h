#pragma once
#include <DirectXCollision.h>
#include "MathHelper.h"
#include "RenderPass.h"

using namespace DirectX;

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexUV;
};

struct Geometry
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;

	// Bounding box of the geometry defined by this submesh. 
	DirectX::BoundingBox Bounds;
};

struct MeshRenderBatchElement
{
	Vertex Primitive;

	// TODO
	// Material, Mesh Options
};

// Mesh batching is a technique used to reduce the number of draw calls.
// It collects the mesh elements depending on the mesh options
// The meshes that have the same options will be processed.
struct MeshRenderBatch : public RenderBatch
{
	std::vector<MeshRenderBatchElement> Elements;
};

// This is the stage to render which has meshes.
class MeshRenderPass : public RenderPass
{
public:
	MeshRenderPass();
	virtual ~MeshRenderPass() = default;

	void AddMeshBatch(MeshRenderBatch&& InBatch);

protected:
	void Process(class RHICommandList& InCommandList) override;

private:
	std::vector<MeshRenderBatch> Batches;
};