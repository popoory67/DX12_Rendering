#include "Commands.h"
#include "Mesh.h"
#include "RenderInterface.h"

RHICommand_Primitive::RHICommand_Primitive(VertexStream&& InStream, unsigned int InSize, unsigned int InStride)
    : StreamResource(std::forward<VertexStream>(InStream))
    , Size(InSize)
    , Stride(InStride)
{

}

RHICommand_Primitive::~RHICommand_Primitive()
{
    StreamResource.clear();
}

void RHICommand_Primitive::Execute(const RHICommandList& InCommandList)
{
    std::shared_ptr<RHIResource> vertices = GRHI->CreateVertexBuffer(Size, Stride);
    void* voidPtr = GRHI->LockBuffer(vertices);
    {
        memcpy(voidPtr, StreamResource.data(), sizeof(StreamResource));
    }
    GRHI->UnlockBuffer(vertices);

    InCommandList.SetStreamResource(vertices);
    InCommandList.DrawIndexedInstanced(vertices, StreamResource.size(), 1, 0, 0, 1); // TODO : test
}