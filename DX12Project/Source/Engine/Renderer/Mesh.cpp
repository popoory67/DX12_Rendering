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

MeshRenderBatch::~MeshRenderBatch()
{
    Elements.clear();
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

MeshRenderPass::~MeshRenderPass()
{
    Batches.clear();
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

    std::vector<Vertex> stream{};

    for (const auto& batch : Batches)
    {
        for (const auto& element : batch.Elements)
        {
            stream.insert(stream.end(), std::make_move_iterator(element.Primitive.begin()), std::make_move_iterator(element.Primitive.end()));
        }
        count += batch.Count;
    }

    TaskGraphSystem::Get().AddTask<RenderCommand>([stream = std::move(stream), stride, count](const RHICommandContext& InContext) mutable
    {
        auto primitiveCommand = RHICommand_SetPrimitive::Create(std::move(stream), stride * count, stride);

        // test
        auto drawPrimitive = RHICommand_DrawPrimitive::Create(count);

        InContext.AddCommand(primitiveCommand);
        InContext.AddCommand(drawPrimitive);

    }, ThreadType::Render);
}