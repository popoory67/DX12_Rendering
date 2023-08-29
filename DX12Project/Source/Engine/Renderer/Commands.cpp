#include "Commands.h"
#include "CommandContext.h"
#include "CommandList.h"
#include "Mesh.h"
#include "RenderInterface.h"

RHICommand_BeginDrawWindow::RHICommand_BeginDrawWindow(RHIViewport* InViewport)
    : Viewport(InViewport)
{

}

void RHICommand_BeginDrawWindow::Execute(const RHICommandContext& InContext)
{
    InContext.GetCommandList().BeginDrawWindow(Viewport);
}

RHICommand_EndDrawWindow::RHICommand_EndDrawWindow(class RHIViewport* InViewport)
    : Viewport(InViewport)
{

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
    std::shared_ptr<RHIResource> vertices = GRHI->CreateVertexBuffer(Size, Stride);
    void* voidPtr = GRHI->LockBuffer(vertices);
    {
        memcpy(voidPtr, StreamResource.data(), sizeof(StreamResource));
    }
    GRHI->UnlockBuffer(vertices);

    //InContext.GetCommandList().SetStreamResource(vertices);
    //InContext.AddResource(std::move(vertices));

    //RHICommand_DrawPrimitive* drawPrimitive = new RHICommand_DrawPrimitive(Size);
    //InContext.AddCommand(std::move(drawPrimitive));
}

RHICommand_DrawPrimitive::RHICommand_DrawPrimitive(unsigned int InSize)
    : Size(InSize)
{

}

void RHICommand_DrawPrimitive::Execute(const RHICommandContext& InContext)
{
    // test
    InContext.GetCommandList().DrawPrimitive(Size, 1, 0, 0);
}
