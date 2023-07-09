#include "stdafx.h"
#include "Mesh.h"
#include "CommandList.h"

MeshRenderPass::MeshRenderPass()
{
    Priority = 5; // test
}

void MeshRenderPass::AddMeshBatch(MeshRenderBatch&& InBatch)
{

}

void MeshRenderPass::Process(RHICommandList& InCommandList)
{

}
