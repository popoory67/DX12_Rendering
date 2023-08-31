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
    InContext.Close();
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

RHICommand_SetPrimitive::RHICommand_SetPrimitive(VertexStream&& InStream, unsigned int InSize, unsigned int InStride)
    : StreamResource(std::forward<VertexStream>(InStream))
    , Size(InSize)
    , Stride(InStride)
{

}

RHICommand_SetPrimitive::~RHICommand_SetPrimitive()
{
    StreamResource.clear();
}

void RHICommand_SetPrimitive::Execute(const RHICommandContext& InContext)
{
    std::shared_ptr<RHIResource> vertexBuffer = GRHI->CreateVertexBuffer(Size, Stride);
    void* voidPtr = GRHI->LockBuffer(vertexBuffer);
    {
        memcpy(voidPtr, StreamResource.data(), Size);
    }
    GRHI->UnlockBuffer(vertexBuffer);

    InContext.GetCommandList().SetStreamResource(vertexBuffer);
    InContext.AddResource(std::move(vertexBuffer));

    unsigned int count = Size / Stride;
    RHICommand_DrawPrimitive* drawPrimitive = new RHICommand_DrawPrimitive(count);
    InContext.AddCommand(std::move(drawPrimitive));
}

RHICommand_DrawPrimitive::RHICommand_DrawPrimitive(unsigned int InCount)
    : Count(InCount)
{

}

void RHICommand_DrawPrimitive::Execute(const RHICommandContext& InContext)
{
    // test
    InContext.GetCommandList().DrawPrimitive(Count, 1, 0, 0);


    RHICommand_DrawPrimitive* drawPrimitive = new RHICommand_DrawPrimitive(Count);
    InContext.AddCommand(std::move(drawPrimitive));
}
