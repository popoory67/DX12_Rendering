#include "Mesh.h"
#include "CommandList.h"
#include "RenderThread.h"
#include "ThreadBase.h"

MeshRenderBatch::MeshRenderBatch(std::vector<MeshRenderBatchElement>&& InMeshStream)
    : Elements(std::forward<std::vector<MeshRenderBatchElement>>(InMeshStream))
{

}

MeshRenderPass::MeshRenderPass()
{
    Priority = 5; // test
}

void MeshRenderPass::AddMeshBatch(MeshRenderBatch&& InBatch)
{
    Batches.emplace_back(std::move(InBatch));
}

void MeshRenderPass::DoTask()
{
    TaskGraphSystem::Get().AddTask<RenderCommand>([meshBatches = std::move(Batches)](const RHICommandList& InCommandList)
    {

    }, ThreadType::Render);
}