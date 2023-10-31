#include <stdexcept>
#include "D3D12Descriptor.h"
#include "D3D12Commands.h"
#include "D3D12View.h"
#include "D3DUtil.h"

D3D12Descriptor::D3D12Descriptor(D3D12Device* InDeivce, D3D12_DESCRIPTOR_HEAP_DESC& InHeapDesc)
	: D3D12Api(InDeivce)
	, Size(0)
{
	ThrowIfFailed(GetDevice()->CreateDescriptorHeap(&InHeapDesc, IID_PPV_ARGS(Heap.GetAddressOf())));

	CPUHandle = Heap->GetCPUDescriptorHandleForHeapStart();
	GPUHandle = Heap->GetGPUDescriptorHandleForHeapStart();
	Size = GetDevice()->GetDescriptorHandleIncrementSize(InHeapDesc.Type);
	Count = InHeapDesc.NumDescriptors;
}

D3D12Descriptor::D3D12Descriptor(D3D12Device* InDeivce, D3D12_DESCRIPTOR_HEAP_TYPE InType, D3D12_DESCRIPTOR_HEAP_FLAGS InFlags, UINT64 InCount)
	: D3D12Api(InDeivce)
	, Size(0)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Flags = InFlags;
	desc.NumDescriptors = (UINT)InCount;
	desc.Type = InType;

	D3D12Descriptor(InDeivce, desc);
}

D3D12Descriptor::~D3D12Descriptor()
{
	Heap.Reset();
}

D3D12_GPU_DESCRIPTOR_HANDLE D3D12Descriptor::GetGpuHandle(UINT64 InIndex) const
{
	assert(Heap);

	if (InIndex >= Count)
	{
		throw std::out_of_range("D3DX12_GPU_DESCRIPTOR_HANDLE");
	}

	D3D12_GPU_DESCRIPTOR_HANDLE handle;
	handle.ptr = GPUHandle.ptr + InIndex * Size;
	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12Descriptor::GetCpuHandle(UINT64 InIndex) const
{
	assert(Heap);

	if (InIndex >= Count)
	{
		throw std::out_of_range("D3DX12_CPU_DESCRIPTOR_HANDLE");
	}

	D3D12_CPU_DESCRIPTOR_HANDLE handle;
	handle.ptr = static_cast<SIZE_T>(CPUHandle.ptr + InIndex * Size);
	return handle;
}

D3D12DescriptorCache::D3D12DescriptorCache(D3D12Device* InDeivce)
	: D3D12Api(InDeivce)
{

}

void D3D12DescriptorCache::SetVertexBuffers(D3D12CommandList& InCommandList, D3D12VertexBufferCache& InCache)
{
	InCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	InCommandList->IASetVertexBuffers(0, 1, &InCache.CurrentVertexBufferView[0]);
}

void D3D12DescriptorCache::SetIndexBuffers(class D3D12CommandList& InCommandList, struct D3D12IndexBufferCache& InCache)
{
	InCommandList->IASetIndexBuffer(&InCache.CurrentIndexBufferView[0]);
}

void D3D12DescriptorCache::SetRenderTargets(D3D12CommandList& InCommandList, D3D12RenderTargetView** InRenderTargets, unsigned int InNumRenderTargets, D3D12DepthStencilView* InDepthStencil)
{
	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandles[MAX_RENDER_TARGETS];

	for (int i = 0; i < InNumRenderTargets; ++i)
	{
		renderTargetHandles[i] = InRenderTargets[i]->GetHandle();
	}

    if (InDepthStencil)
    {
		const D3D12_CPU_DESCRIPTOR_HANDLE& depthStencilHandle = InDepthStencil->GetHandle();
		InCommandList->OMSetRenderTargets(InNumRenderTargets, renderTargetHandles,
            0, &depthStencilHandle);
    }
    else
    {
		InCommandList->OMSetRenderTargets(InNumRenderTargets, renderTargetHandles,
            0, nullptr);
    }
}
