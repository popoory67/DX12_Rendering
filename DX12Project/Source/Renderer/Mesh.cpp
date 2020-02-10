#include "stdafx.h"
#include "Mesh.h"
#include "GeometryGenerator.h"
#include "D3D12BinaryLargeObject.h"
#include "D3D12Resource.h"
#include "D3D12Commands.h"
#include "D3D12Device.h"

Primitive::Primitive()
{
	GeometryData = new GeometryUploader();
}

Primitive::~Primitive()
{
	delete(GeometryData);
}

void Primitive::Build(D3D12Device* InDevice, D3D12CommandList* InCommandList)
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3);

	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)box.Indices32.size();
	boxSubmesh.StartIndexLocation = 0;
	boxSubmesh.BaseVertexLocation = 0;

	std::vector<GeometryGenerator::Vertex> vertices(box.Vertices.size());

	for (size_t i = 0; i < box.Vertices.size(); ++i)
	{
		vertices[i].Position = box.Vertices[i].Position;
		vertices[i].Normal = box.Vertices[i].Normal;
		vertices[i].TexC = box.Vertices[i].TexC;
	}

	std::vector<std::uint16_t> indices = box.GetIndices16();

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(GeometryGenerator::Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	GeometryData->Name = "boxGeo";

	GeometryData->VertexBufferCPU->CreateBlob(vbByteSize);
	CopyMemory(GeometryData->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	GeometryData->IndexBufferCPU->CreateBlob(ibByteSize);
	CopyMemory(GeometryData->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	GeometryData->VertexBufferGPU->CreateDefaultBuffer(InDevice, InCommandList, vertices.data(), vbByteSize);
	GeometryData->IndexBufferGPU->CreateDefaultBuffer(InDevice, InCommandList, indices.data(), ibByteSize);

	GeometryData->VertexByteStride = sizeof(GeometryGenerator::Vertex);
	GeometryData->VertexBufferByteSize = vbByteSize;
	GeometryData->IndexFormat = DXGI_FORMAT_R16_UINT;
	GeometryData->IndexBufferByteSize = ibByteSize;

	GeometryData->DrawArgs["box"] = boxSubmesh;

//	mGeometries[GeometryData->Name] = std::move(GeometryData);
}

D3D12_VERTEX_BUFFER_VIEW Primitive::VertexBufferView() const
{
	assert(GeometryData);
	return GeometryData->VertexBufferView();
}

D3D12_INDEX_BUFFER_VIEW Primitive::IndexBufferView() const
{
	assert(GeometryData);
	return GeometryData->IndexBufferView();
}
