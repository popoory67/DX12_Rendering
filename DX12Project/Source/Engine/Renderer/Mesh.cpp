#include "Mesh.h"
#include "CommandList.h"

MeshRenderPass::MeshRenderPass()
{
    Priority = 5; // test
}

void MeshRenderPass::AddMeshBatch(MeshRenderBatch&& InBatch)
{
    // MeshRenderBatchElement
    // MeshRenderBatch

    //MeshRenderBatch meshBatch;
    //MeshRenderBatchElement& element = meshBatch.Elements[0];
}

void MeshRenderPass::Process(RHICommandList& InCommandList)
{

}
