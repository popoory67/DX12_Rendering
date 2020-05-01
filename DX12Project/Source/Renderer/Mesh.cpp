#include "stdafx.h"
#include "Mesh.h"
#include "D3D12BinaryLargeObject.h"
#include "D3D12Resource.h"
#include "D3D12Commands.h"
#include "D3D12Device.h"
#include "D3D12Rendering.h"

PrimitiveComponent::PrimitiveComponent()
{
	GeometryData = new GeometryUploader();
}

PrimitiveComponent::~PrimitiveComponent()
{
	delete(GeometryData);
}

void PrimitiveComponent::CreateMesh(std::string InPath)
{
	// Test box
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3);

	Build(box);
}

void PrimitiveComponent::Build(GeometryGenerator::MeshData& InMeshData)
{
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

	GeometryData->Name = "0";

	GeometryData->VertexBufferCPU->CreateBlob(vbByteSize);
	CopyMemory(GeometryData->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	GeometryData->IndexBufferCPU->CreateBlob(ibByteSize);
	CopyMemory(GeometryData->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	GeometryData->VertexBufferGPU->CreateDefaultBuffer(vertices.data(), vbByteSize);
	GeometryData->IndexBufferGPU->CreateDefaultBuffer(indices.data(), ibByteSize);

	GeometryData->VertexByteStride = sizeof(GeometryGenerator::Vertex);
	GeometryData->VertexBufferByteSize = vbByteSize;
	GeometryData->IndexFormat = DXGI_FORMAT_R16_UINT;
	GeometryData->IndexBufferByteSize = ibByteSize;

	IndexCount += (UINT)InMeshData.Indices32.size();

	// Submesh using bounding box
	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)InMeshData.Indices32.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	DrawArgs[0] = submesh;
}

D3D12_VERTEX_BUFFER_VIEW& PrimitiveComponent::VertexBufferView() const
{
	assert(GeometryData);
	return GeometryData->VertexBufferView();
}

D3D12_INDEX_BUFFER_VIEW& PrimitiveComponent::IndexBufferView() const
{
	assert(GeometryData);
	return GeometryData->IndexBufferView();
}
