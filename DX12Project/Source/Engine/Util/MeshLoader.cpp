#include "MeshLoader.h"
#include "Material.h"
#include "Mesh.h"

// Test dx loader
#include "DXMeshLoader.h"
using ProcessPlatform = DXMeshLoader;

MeshLoader* MeshLoader::Create()
{
    MeshLoader* loader = ProcessPlatform::CreateLoader();
    return loader;
}
