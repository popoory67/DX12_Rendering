#include "Commands.h"
#include "CommandContext.h"
#include "CommandList.h"
#include "Mesh.h"
#include "RenderInterface.h"
#include <iostream>

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

RHICommand_BeginRender::RHICommand_BeginRender(XMMATRIX&& InWorldViewProjection)
    : WorldViewProjection(std::move(InWorldViewProjection))
{

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

        memcpy(buffer + size, &constantData, sizeof(constantData));
    }
    GRHI->UnlockBuffer(constantBuffer);

    //OutputDebugStringA("//--------------------test");

    InContext.GetCommandList().AddShaderReference(0, constantBuffer);
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
    UINT8* voidPtr = GRHI->LockBuffer(vertexBuffer);
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
    //DrawIndexedInstanced
}
