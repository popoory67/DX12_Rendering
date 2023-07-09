#include "stdafx.h"
#include "PrimitiveComponent.h"
#include "Util.h"
#include "RenderInterface.h"

PrimitiveComponent::PrimitiveComponent()
{
	PrimitiveData = new PrimitiveUploadData();
}

PrimitiveComponent::~PrimitiveComponent()
{
	SafeDelete(PrimitiveData);
}

void PrimitiveComponent::CreateMesh(const std::string& InPath)
{
	// Test box
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3);

	Build(box);
}

void PrimitiveComponent::Update()
{
}

void PrimitiveComponent::Build(GeometryGenerator::MeshData& InMeshData)
{
	assert(PrimitiveData);
	
	// Vertices
	std::vector<GeometryGenerator::Vertex> vertices(InMeshData.Vertices.size());

	for (size_t i = 0; i < InMeshData.Vertices.size(); ++i)
	{
		vertices[i].Position = InMeshData.Vertices[i].Position;
		vertices[i].Normal = InMeshData.Vertices[i].Normal;
		vertices[i].TexC = InMeshData.Vertices[i].TexC;
	}

	// Indices
	std::vector<std::uint16_t> indices = InMeshData.GetIndices16();

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(GeometryGenerator::Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	//PrimitiveData->VertexBufferCPU->CreateBlob(vbByteSize);
	//CopyMemory(PrimitiveData->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	//PrimitiveData->IndexBufferCPU->CreateBlob(ibByteSize);
	//CopyMemory(PrimitiveData->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	//RenderInterface::CreateDefaultBuffer(PrimitiveData->VertexBufferGPU, vertices.data(), vbByteSize);
	//RenderInterface::CreateDefaultBuffer(PrimitiveData->IndexBufferGPU, indices.data(), ibByteSize);

	PrimitiveData->VertexByteStride = sizeof(GeometryGenerator::Vertex);
	PrimitiveData->VertexBufferByteSize = vbByteSize;
	PrimitiveData->IndexFormat = DXGI_FORMAT_R16_UINT;
	PrimitiveData->IndexBufferByteSize = ibByteSize;
	PrimitiveData->IndexCount += (UINT)InMeshData.Indices32.size();

	//auto func = [vertices, vbByteSize, indices, ibByteSize](CommandList& InCommandList)
	//{
	//	PrimitiveCommand* pCommand = new PrimitiveCommand();
	//	assert(pCommand);
	//	
	//	pCommand->VertexBufferCPU->CreateBlob(vbByteSize);
	//	CopyMemory(pCommand->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	//	pCommand->IndexBufferCPU->CreateBlob(ibByteSize);
	//	CopyMemory(pCommand->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);
	//};

	// 추가
	
	
	// Submesh using bounding box
	//SubmeshGeometry submesh;
	//submesh.IndexCount = (UINT)InMeshData.Indices32.size();
	//submesh.StartIndexLocation = 0;
	//submesh.BaseVertexLocation = 0;

	//DrawArgs[0] = submesh;
}
