#pragma once
#include "RenderComponent.h"
#include "ObjectCommand.h"

struct BPrimitiveUploadData : public BUploadData
{
	// DrawIndexedInstanced parameters.
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;

	// Data about the buffers.
	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT IndexBufferByteSize = 0;
};

struct BPrimitiveCommand : public BObjectCommand
{
	class D3D12BinaryLargeObject* VertexBufferCPU = nullptr;
	class D3D12BinaryLargeObject* IndexBufferCPU = nullptr;

	class D3D12DefaultResource* VertexBufferGPU = nullptr;
	class D3D12DefaultResource* IndexBufferGPU = nullptr;
};

class BPrimitiveComponent : public BRenderComponent
{
public:
	BPrimitiveComponent();
	virtual ~BPrimitiveComponent();
	
	UINT GetIndex() const { return Index; }

	void CreateMesh(const std::string& InPath);

protected:
	void Update() override;

private:
	void Build(struct GeometryGenerator::MeshData& InMeshData);

public:
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

private:

//	std::unordered_map<unsigned, Geometry> DrawArgs;

	UINT Index = 0;

	BPrimitiveUploadData* PrimitiveData = nullptr;
};