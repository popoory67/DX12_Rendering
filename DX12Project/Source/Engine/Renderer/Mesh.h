#pragma once
#include "MathHelper.h"
#include "RenderPass.h"
#include "Primitive.h"
#include "MathHelper.h"
#include "MaterialComponent.h"
#include <vector>

using VertexStream = std::vector<Vertex>;
using IndexStream = std::vector<unsigned int>;

struct MeshRenderBatchElement
{
	VertexStream Vertices;
	IndexStream Indices;
	unsigned int Stride;

	MaterialProxy* MaterialShaderProxy = nullptr;

	MeshRenderBatchElement() = default;
	MeshRenderBatchElement(const MeshRenderBatchElement&) = default;
	MeshRenderBatchElement& operator=(const MeshRenderBatchElement&) = default;

	MeshRenderBatchElement(MeshRenderBatchElement&& InElement)
	{
		Vertices = std::move(InElement.Vertices);
		Indices = std::move(InElement.Indices);
		Stride = InElement.Stride;
	}

	MeshRenderBatchElement& operator=(MeshRenderBatchElement&& InElement)
	{
		Vertices = std::move(InElement.Vertices);
		Indices = std::move(InElement.Indices);
		Stride = InElement.Stride;
		return *this;
	}

	~MeshRenderBatchElement()
	{
		Vertices.clear();
		Indices.clear();
	}
};

// Mesh batching is a technique used to reduce the number of draw calls.
// It collects the mesh elements depending on the mesh options.
// The meshes that have the same options will be processed.
struct MeshRenderBatch : public RenderBatch
{
	MeshRenderBatch();
	MeshRenderBatch(std::vector<MeshRenderBatchElement>&& InMeshStream, unsigned int InCount);

	MeshRenderBatch(MeshRenderBatch&& InBatch)
		: RenderBatch(std::move(InBatch))
	{
		Count = InBatch.Count;
		Elements = std::move(InBatch.Elements);
	}

	MeshRenderBatch& operator=(MeshRenderBatch&& InBatch)
	{
		Count = InBatch.Count;
		Elements = std::move(InBatch.Elements);
	}

	virtual ~MeshRenderBatch();

	void AddElements(std::vector<MeshRenderBatchElement>&& InMeshStream);
	void AddElement(const MeshRenderBatchElement& InMeshElement);
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
	virtual ~MeshRenderPass();
	MeshRenderPass(MeshRenderPass&& InPass)
	{
		Batches = std::move(InPass.Batches);
	}

	MeshRenderPass& operator=(MeshRenderPass&& InPass)
	{
		Batches = std::move(InPass.Batches);
	}

	void AddMeshBatch(MeshRenderBatch&& InBatch);
	void DoTask() override;

private:
	std::vector<MeshRenderBatch> Batches;
};