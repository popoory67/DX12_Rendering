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

struct SubmeshGeometry
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;

	// Bounding box of the geometry defined by this submesh. 
	DirectX::BoundingBox Bounds;
};

struct GeometryUploader
{
	size_t Id = 0;

	// Give it a name so we can look it up by name.
	std::string Name;

	// System memory copies.  Use Blobs because the vertex/index format can be generic.
	// It is up to the client to cast appropriately.  
	class D3D12BinaryLargeObject* VertexBufferCPU = nullptr;
	class D3D12BinaryLargeObject* IndexBufferCPU = nullptr;

	class D3D12DefaultResource* VertexBufferGPU = nullptr;
	class D3D12DefaultResource* IndexBufferGPU = nullptr;

	// 업로드 버퍼
// 	class D3D12Resource* VertexBufferUploader = nullptr;
// 	class D3D12Resource* IndexBufferUploader = nullptr;

	// Data about the buffers.
	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT IndexBufferByteSize = 0;

	// 정점 버퍼 서술자
	D3D12_VERTEX_BUFFER_VIEW& VertexBufferView() const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress().value();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW& IndexBufferView() const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress().value();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;

		return ibv;
	}

	// We can free this memory after we finish upload to the GPU.
	void DisposeUploaders()
	{
// 		VertexBufferUploader = nullptr;
// 		IndexBufferUploader = nullptr;
	}
};

class PrimitiveComponent : public RenderComponent
{
public:
	PrimitiveComponent();
	virtual ~PrimitiveComponent();

	D3D12_VERTEX_BUFFER_VIEW& VertexBufferView() const;
	D3D12_INDEX_BUFFER_VIEW& IndexBufferView() const;

	UINT GetIndexCount() const { return IndexCount; }
	UINT GetStartIndexLocation() const { return StartIndexLocation; }
	int GetBaseVertexLocation() const { return BaseVertexLocation; }
	UINT GetIndex() const { return Index; }

	void CreateMesh(std::string InPath);

private:
	void Build(struct GeometryGenerator::MeshData& InMeshData);

public:
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

private:
	// DrawIndexedInstanced parameters.
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;

	GeometryUploader* GeometryData = nullptr;

	std::unordered_map<unsigned, SubmeshGeometry> DrawArgs;

	size_t Index = 0;
};