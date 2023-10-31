#include "D3D12View.h"
#include "D3D12Descriptor.h"
#include "D3D12RenderInterface.h"

D3D12View::D3D12View(D3D12Descriptor* InDescriptor)
    : Descriptor(InDescriptor)
{

}

D3D12View::~D3D12View()
{
    SafeDelete(Descriptor);
}

const D3D12_CPU_DESCRIPTOR_HANDLE& D3D12View::GetHandle(UINT64 InIndex /*= 0*/) const
{
    return Descriptor->GetCpuHandle(InIndex);
}

D3D12RenderTargetView::D3D12RenderTargetView(D3D12Resource* InResource, D3D12Descriptor* InDescriptor)
    : Parent(InResource)
    , D3D12View(InDescriptor)
{

}

D3D12RenderTargetView::~D3D12RenderTargetView()
{
    
}

void D3D12RenderTargetView::Reset()
{
    D3D12Resource* resource = D3D12RHI::Cast(TargetResource);
    assert(resource);

    resource->Reset();
}

D3D12DepthStencilView::D3D12DepthStencilView(D3D12Resource* InResource, class D3D12Descriptor* InDescriptor)
    : Parent(InResource)
    , D3D12View(InDescriptor)
{

}

void D3D12DepthStencilView::Reset()
{
    D3D12Resource* resource = D3D12RHI::Cast(TargetResource);
    assert(resource);

    resource->Reset();
}
