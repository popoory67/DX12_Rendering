#include "Commands.h"
#include "CommandContext.h"
#include "CommandList.h"
#include "Mesh.h"
#include "RenderInterface.h"
#include "CommandContext.h"
#include <iostream>

RHICommand_BeginDrawWindow::RHICommand_BeginDrawWindow(RHIViewport* InViewport)
    : Viewport(InViewport)
{
    Priority = PipelinePrioirty::PreRender;
}

void RHICommand_BeginDrawWindow::Execute(const RHICommandContext& InContext)
{
    InContext.GetCommandList().BeginDrawWindow(Viewport);
}

RHICommand_EndDrawWindow::RHICommand_EndDrawWindow(class RHIViewport* InViewport)
    : Viewport(InViewport)
{
    Priority = PipelinePrioirty::EndRender;
}

void RHICommand_EndDrawWindow::Execute(const RHICommandContext& InContext)
{
    InContext.GetCommandList().EndRender();
    InContext.GetCommandList().EndDrawWindow(Viewport);
}

RHICommand_BeginRender::RHICommand_BeginRender(XMMATRIX&& InWorldViewProjection)
    : WorldViewProjection(std::move(InWorldViewProjection))
{
    Priority = PipelinePrioirty::BeginRender;
}

void RHICommand_BeginRender::Execute(const RHICommandContext& InContext)
{
    InContext.GetCommandList().BeginRender();

    const int frameCount = 2; // test
    const int size = sizeof(ConstantBuffer) * frameCount;

    // Mapped buffer that is truly handled on Rendering
    RHIResource* constantBuffer = GRHI->CreateBuffer(size, sizeof(XMMATRIX));
    UINT8* buffer = GRHI->LockBuffer(constantBuffer);
    {
        // Mapping constant data
        ConstantBuffer constantData;
        XMStoreFloat4x4(&constantData.WorldViewProj, XMMatrixTranspose(WorldViewProjection));

        memcpy(buffer /*+ size*/, &constantData, sizeof(constantData));
    }
    GRHI->UnlockBuffer(constantBuffer);

    InContext.GetCommandList().AddShaderReference(0, constantBuffer);
    InContext.GetCommandList().AddResource(std::move(constantBuffer));
}

RHICommand_SetPrimitive::RHICommand_SetPrimitive(std::vector<Vertex>&& InVertexStream, std::vector<unsigned int>&& InIndexStream, unsigned int InStride)
    : VertexStreamResource(std::forward<std::vector<Vertex>>(InVertexStream))
    , IndexStreamResource(std::forward<std::vector<unsigned int>>(InIndexStream))
    , Stride(InStride)
{
    Priority = PipelinePrioirty::DrawSetting;
}

RHICommand_SetPrimitive::~RHICommand_SetPrimitive()
{
    VertexStreamResource.clear();
}

void RHICommand_SetPrimitive::Execute(const RHICommandContext& InContext)
{
    const unsigned int verticesSize = VertexStreamResource.size() * Stride;
    const unsigned int indicesSize = IndexStreamResource.size() * sizeof(unsigned int);

    RHIResource* vertexBuffer = GRHI->CreateVertexBuffer(verticesSize, indicesSize, Stride);
    UINT8* voidPtr = GRHI->LockBuffer(vertexBuffer);
    {
        memcpy(voidPtr, VertexStreamResource.data(), verticesSize);
        voidPtr += verticesSize;

        memcpy(voidPtr, IndexStreamResource.data(), indicesSize);
    }
    GRHI->UnlockBuffer(vertexBuffer);

    InContext.GetCommandList().SetStreamResource(vertexBuffer, indicesSize);
    InContext.GetCommandList().AddResource(std::move(vertexBuffer));
}

RHICommand_DrawPrimitive::RHICommand_DrawPrimitive(unsigned int InCount)
    : Count(InCount)
{
    Priority = PipelinePrioirty::Draw;
}

void RHICommand_DrawPrimitive::Execute(const RHICommandContext& InContext)
{
    // test
    //InContext.GetCommandList().DrawPrimitive(Count, 1, 0, 0);
    InContext.GetCommandList().DrawIndexedInstanced(Count, 1, 0, 0, 0);
}

RHICommand_SetShaderResource::RHICommand_SetShaderResource(TextureSettings* InSettings)
    : Settings(InSettings)
{
    Priority = PipelinePrioirty::BeginRender;
}

void RHICommand_SetShaderResource::Execute(const RHICommandContext& InContext)
{
    // DirectX 12 supports normally 4 channels.
    // Because the 4 channels format is much more optimized on GPU devices than 3 channels.
    // Therefore, to change a texture format to 4 channels for a hardware compatibility is pretty usual.
    size_t rgbaSize = Settings->Width * Settings->Height * 4;

    size_t rowPitch = Settings->Width * Settings->Channels;
    size_t size = rowPitch * Settings->Height;

    // A texture
    RHIResource* textureResource = GRHI->CreateTexture(Settings);

    // A staging buffer that is a temporarily allocated memory for moving CPU memory to GPU memory.
    RHIResource* stagingBuffer = GRHI->CreateBuffer(rgbaSize, rowPitch);
    {
        // A raw texture data to the staging
        UINT8* buffer = GRHI->LockBuffer(stagingBuffer);
        {
            memcpy(buffer, Settings->TextureData, size);
        }
        GRHI->UnlockBuffer(stagingBuffer);
    }

    // Copy the CPU texture to the GPU texture. (Staging -> Texture)
    InContext.GetCommandList().CopyResourceRegion(textureResource, stagingBuffer);

    InContext.GetCommandList().AddResource(stagingBuffer);
    InContext.GetCommandList().AddResource(textureResource);
}

RHICommand_SetPipelineState::RHICommand_SetPipelineState(GraphicsPipelineState::Key InPipelineStateId)
    : PipelineStateId(InPipelineStateId)
{

}

RHICommand_SetPipelineState::~RHICommand_SetPipelineState()
{
    Shaders.clear();
}

void RHICommand_SetPipelineState::AddShader(ShaderBinding* InShaderBinding)
{
    Shaders.emplace_back(InShaderBinding);
}

void RHICommand_SetPipelineState::Execute(const RHICommandContext& InContext)
{
    const GraphicsPipelineState::PSOStream& psoStream = GraphicsPipelineState::GetPSOCache(PipelineStateId);
    if (!psoStream.empty())
    {
        InContext.GetCommandList().SetPipelineState(PipelineStateId, psoStream);
    }
    else
    {
        InContext.GetCommandList().CreateAndAddPipelineState(std::move(Shaders));
    }
}
