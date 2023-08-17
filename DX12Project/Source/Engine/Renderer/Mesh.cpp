#include "Mesh.h"
#include "ThreadBase.h"
#include "Commands.h"
#include "RenderThread.h"


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
    RHICommand_Primitive* primitiveCommand = new RHICommand_Primitive(std::move(Batches));
    TaskGraphSystem::Get().AddTask<RenderCommand>([command = std::move(primitiveCommand)](const RHICommandList& InCommandList)
    {
        InCommandList.AddCommand(std::move(command));

    }, ThreadType::Render);
}