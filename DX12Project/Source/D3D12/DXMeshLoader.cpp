#include "DXMeshLoader.h"
#include "D3DUtil.h"
#include "Primitive.h"

DXMeshLoader::DXMeshLoader()
{
    ObjCache = std::make_unique<WaveFrontReader<unsigned int>>();
}

DXMeshLoader::~DXMeshLoader()
{

}

MeshLoader* DXMeshLoader::CreateLoader()
{
    return new DXMeshLoader();
}

void DXMeshLoader::LoadObj(const wchar_t* InFileName)
{
    assert(InFileName != nullptr);
    ThrowIfFailed(ObjCache->Load(InFileName));
}

void DXMeshLoader::GetVertices(std::vector<Vertex>& OutVertices)
{
    Vertex vertex;

    for (const auto& it : ObjCache->vertices)
    {
        vertex.Position = it.position;
        vertex.Normal = it.normal;
        vertex.TextureCoordinate = it.textureCoordinate;

        OutVertices.emplace_back(vertex);
    }
}

void DXMeshLoader::GetIndices(std::vector<unsigned int>& OutIndices)
{
    OutIndices = std::move(ObjCache->indices);
}
