#include "Mesh.h"
#include "ThreadBase.h"
#include "Commands.h"
#include "CommandContext.h"
#include "CommandList.h"
#include "RenderThread.h"

MeshRenderBatch::MeshRenderBatch()
    : Count(0)
{

}

MeshRenderBatch::MeshRenderBatch(std::vector<MeshRenderBatchElement>&& InMeshStream, unsigned int InCount)
    : Elements(std::forward<std::vector<MeshRenderBatchElement>>(InMeshStream))
    , Count(InCount)
{
    
}

void MeshRenderBatch::AddElements(std::vector<MeshRenderBatchElement>&& InMeshStream)
{
    for (const auto& element : InMeshStream)
    {
        Count += element.Primitive.size();
    }
    Elements.insert(Elements.end(), std::make_move_iterator(InMeshStream.begin()), std::make_move_iterator(InMeshStream.end()));
}

void MeshRenderBatch::AddElement(MeshRenderBatchElement&& InMeshElement)
{
    Count += InMeshElement.Primitive.size();
    Elements.emplace_back(std::move(InMeshElement));
}

unsigned int MeshRenderBatch::GetStride() const
{
    if (Elements.empty())
    {
        return 0;
    }

    return Elements[0].Stride;
}

MeshRenderPass::MeshRenderPass()
{
    Priority = 5; // TODO : test
}

void MeshRenderPass::AddMeshBatch(MeshRenderBatch&& InBatch)
{
    Batches.emplace_back(std::move(InBatch));
}

void MeshRenderPass::DoTask()
{
    if (Batches.empty())
    {
        return;
    }

    unsigned int stride = Batches[0].GetStride();
    unsigned int count = 0;

    VertexStream stream{};

    for (const auto& batch : Batches)
    {
        for (const auto& element : batch.Elements)
        {
            stream.insert(stream.end(), std::make_move_iterator(element.Primitive.begin()), std::make_move_iterator(element.Primitive.end()));
        }
        count += batch.Count;
    }

    RHICommand_SetPrimitive* primitiveCommand = new RHICommand_SetPrimitive(std::move(stream), stride * count, stride);

    TaskGraphSystem::Get().AddTask<RenderCommand>([command = std::move(primitiveCommand)](const RHICommandContext& InContext)
    {
        InContext.AddCommand(std::move(command));

    }, ThreadType::Render);
}