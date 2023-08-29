#include "RenderInterface.h"

RHI* GRHI = nullptr;

// TODO
// test
#include "D3D12RenderInterface.h"
using ProcessPlatform = D3D12RHI;

RHIRenderTargetInfo::RHIRenderTargetInfo(RHIResource* InTargetResource)
    : TargetResource(InTargetResource)
{

}

RHIRenderTargetInfo::~RHIRenderTargetInfo()
{
    SafeDelete(TargetResource);
}

RHIResource* RHIRenderTargetInfo::GetTexture() const
{
    return TargetResource;
}

RHIDepthStencilInfo::RHIDepthStencilInfo(RHIResource* InTargetResource)
{

}

RHIDepthStencilInfo::~RHIDepthStencilInfo()
{
    SafeDelete(TargetResource);
}

RHIResource* RHIDepthStencilInfo::GetTexture() const
{
    return TargetResource;
}

namespace RenderInterface
{
    RHI* GetPlatformRHI()
    {
        return new ProcessPlatform();
    }
};
