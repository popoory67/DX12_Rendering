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
        Count += element.Vertices.size();
    }
    Elements.insert(Elements.end(), std::make_move_iterator(InMeshStream.begin()), std::make_move_iterator(InMeshStream.end()));
}

void MeshRenderBatch::AddElement(MeshRenderBatchElement&& InMeshElement)
{
    Count += InMeshElement.Vertices.size();
    Elements.emplace_back(std::move(InMeshElement));
}

void MeshRenderBatch::AddElement(const MeshRenderBatchElement& InMeshElement)
{
    Count += InMeshElement.Vertices.size();
    Elements.emplace_back(InMeshElement);
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
    Type = RenderPassType::Base;
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

    VertexStream vertexStream{};
    IndexStream indexStream{};

    // test
    TextureSettings* resource = nullptr;
    ShaderBinding* vsShader = nullptr;
    ShaderBinding* fsShader = nullptr;

    for (const auto& batch : Batches)
    {
        for (const auto& element : batch.Elements)
        {
            vertexStream.insert(vertexStream.end(), std::make_move_iterator(element.Vertices.begin()), std::make_move_iterator(element.Vertices.end()));
            indexStream.insert(indexStream.end(), std::make_move_iterator(element.Indices.begin()), std::make_move_iterator(element.Indices.end()));

            // test
            resource = element.MaterialShaderProxy->TextureInfo;
            vsShader = &element.MaterialShaderProxy->ShaderInfo_VS;
            fsShader = &element.MaterialShaderProxy->ShaderInfo_FS;

            // PSO

        }
    }

    int indicesSize = indexStream.size();

    auto primitiveCommand = RHICommand_SetPrimitive::Create(
        std::move(vertexStream),
        std::move(indexStream),
        stride);

    // TODO
    // test
    auto drawPrimitive = RHICommand_DrawPrimitive::Create(indicesSize);
    {
        GetCommandContext().AddCommand(primitiveCommand);
        GetCommandContext().AddCommand(drawPrimitive);
    }

    // shader test
    auto shaderCommand = RHICommand_SetShaderResource::Create(resource);
    {
        shaderCommand->AddShader(vsShader);
        shaderCommand->AddShader(fsShader);

        GetCommandContext().AddCommand(shaderCommand);
    }
}