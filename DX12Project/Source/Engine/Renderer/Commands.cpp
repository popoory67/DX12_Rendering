#include "Commands.h"
#include "CommandContext.h"
#include "CommandList.h"
#include "Mesh.h"
#include "RenderInterface.h"

RHICommand_BeginDrawWindow::RHICommand_BeginDrawWindow(RHIViewport* InViewport)
    : Viewport(InViewport)
{
    Priority = CommandPriority::First;
}

void RHICommand_BeginDrawWindow::Execute(const RHICommandContext& InContext)
{
    InContext.GetCommandList().BeginDrawWindow(Viewport);
}

RHICommand_EndDrawWindow::RHICommand_EndDrawWindow(class RHIViewport* InViewport)
    : Viewport(InViewport)
{
    Priority = CommandPriority::Last;
}

void RHICommand_EndDrawWindow::Execute(const RHICommandContext& InContext)
{
    InContext.GetCommandList().EndDrawWindow(Viewport);
}

RHICommand_SetRenderTargets::RHICommand_SetRenderTargets(RHIRenderTargetInfo* InRenderTargets, unsigned int InNumRenderTargets, RHIResource* InDepthStencil)
    : RenderTargets(InRenderTargets)
    , NumRenderTargets(InNumRenderTargets)
    , DepthStencil(InDepthStencil)
{

}

void RHICommand_SetRenderTargets::Execute(const RHICommandContext& InContext)
{
    InContext.GetCommandList().SetRenderTargets(RenderTargets, NumRenderTargets, DepthStencil);
}

RHICommand_SetRenderTargets::~RHICommand_SetRenderTargets()
{

}

RHICommand_SetPrimitive::RHICommand_SetPrimitive(std::vector<Vertex>&& InVertexStream, std::vector<unsigned int>&& InIndexStream, unsigned int InSize, unsigned int InStride)
    : VertexStreamResource(std::forward<std::vector<Vertex>>(InVertexStream))
    , IndexStreamResource(std::forward<std::vector<unsigned int>>(InIndexStream))
    , Size(InSize)
    , Stride(InStride)
{

}

RHICommand_SetPrimitive::~RHICommand_SetPrimitive()
{
    VertexStreamResource.clear();
}

void RHICommand_SetPrimitive::Execute(const RHICommandContext& InContext)
{
    const UINT indicesSize = sizeof(IndexStreamResource) * sizeof(unsigned int);

    RHIResource* vertexBuffer = GRHI->CreateVertexBuffer(Size, indicesSize, Stride);
    UINT8* voidPtr = static_cast<UINT8*>(GRHI->LockBuffer(vertexBuffer)); // test
    {
        memcpy(voidPtr, VertexStreamResource.data(), Size);
        voidPtr += Size;

        memcpy(voidPtr, IndexStreamResource.data(), indicesSize);
        voidPtr += indicesSize;
    }
    GRHI->UnlockBuffer(vertexBuffer);

    InContext.GetCommandList().SetStreamResource(vertexBuffer, indicesSize);
    InContext.GetCommandList().AddResource(std::move(vertexBuffer));
}

RHICommand_DrawPrimitive::RHICommand_DrawPrimitive(unsigned int InCount)
    : Count(InCount)
{

}

void RHICommand_DrawPrimitive::Execute(const RHICommandContext& InContext)
{
    // test
    InContext.GetCommandList().DrawPrimitive(Count, 1, 0, 0);
}
