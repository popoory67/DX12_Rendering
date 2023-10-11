#pragma once
#include <vector>

class MeshLoader
{
public:
	MeshLoader() = default;
	virtual ~MeshLoader() = default;

	static MeshLoader* Create();

	virtual void LoadObj(const wchar_t* InFileName) = 0;
	virtual void GetVertices(std::vector<struct Vertex>& OutVertices) = 0;
	virtual void GetIndices(std::vector<unsigned int>& OutIndices) = 0;
};