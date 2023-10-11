#pragma once
#include "MeshLoader.h"
#include "DirectXMesh.h"
#include "WaveFrontReader.h"

class DXMeshLoader : public MeshLoader
{
public:
	DXMeshLoader();
	virtual ~DXMeshLoader();

	static MeshLoader* CreateLoader();

	void LoadObj(const wchar_t* InFileName) final override;
	void GetVertices(std::vector<struct Vertex>& OutVertices) final override;
	void GetIndices(std::vector<unsigned int>& OutIndices) final override;

private:
	std::unique_ptr<WaveFrontReader<unsigned int>> ObjCache;
};

/*
		std::vector<struct Vertex>& OutVertices,
		std::vector<unsigned int>& OutIndices,
		std::vector<uint32_t>& OutAttributes,
		std::vector<struct Material>& OutMaterials
*/