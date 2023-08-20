#pragma once
#include <DirectXCollision.h>
#include <vector>
#include "MathHelper.h"
#include "RenderPass.h"

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

// TODO
// VertexStream should be flexible, not just apply the struct Vertex.
using VertexStream = std::vector<Vertex>;

struct MeshRenderBatchElement
{
	VertexStream Primitive;
	unsigned int Stride;

	// TODO
	// Material, Mesh Options, Proxy
};

// Mesh batching is a technique used to reduce the number of draw calls.
// It collects the mesh elements depending on the mesh options.
// The meshes that have the same options will be processed.
struct MeshRenderBatch : public RenderBatch
{
	MeshRenderBatch();
	MeshRenderBatch(std::vector<MeshRenderBatchElement>&& InMeshStream, unsigned int InCount);
	virtual ~MeshRenderBatch() = default;

	void AddElements(std::vector<MeshRenderBatchElement>&& InMeshStream);
	void AddElement(MeshRenderBatchElement&& InMeshElement);
	unsigned int GetStride() const;

	unsigned int Count;
	std::vector<MeshRenderBatchElement> Elements;
};

// This is the stage to render which has meshes.
class MeshRenderPass : public RenderPass
{
public:
	MeshRenderPass();
	virtual ~MeshRenderPass() = default;

	void AddMeshBatch(MeshRenderBatch&& InBatch);
	void DoTask() override;

private:
	std::vector<MeshRenderBatch> Batches;
};