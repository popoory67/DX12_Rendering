#include "D3D12Commands.h"
#include "D3D12Device.h"
#include "D3D12RenderInterface.h"
#include "D3D12Fence.h"
#include "D3D12Descriptor.h"
#include "D3D12Resource.h"
#include "D3D12PipelineState.h"
#include "D3D12RootSignature.h"
#include "D3DUtil.h"
#include <optional>

D3D12CommandList::D3D12CommandList(D3D12Device* InDevice)
	: D3D12Api(InDevice)
	, StateCache(InDevice)
{

}

D3D12CommandList::~D3D12CommandList()
{
    SafeDelete(CommandListAllocator);
    SafeDelete(Fence);
}

void D3D12CommandList::Reset()
{
	CommandListAllocator->Reset();

	ThrowIfFailed(CommandList->Reset(CommandListAllocator->Get(),/* OpaquePipelineStateObject.Get()*/ nullptr));

	bClosed = false;
}

void D3D12CommandList::Close()
{
	if (!bClosed)
	{
		FlushTransitions();

		ThrowIfFailed(CommandList->Close());

		bClosed = true;
	}
}

ID3D12CommandList* D3D12CommandList::GetCommandLists()
{
	assert(CommandList || CommandList.Get());
	return CommandList.Get();
}

D3D12PipelineStateCache& D3D12CommandList::GetStateCache()
{
    return StateCache;
}

void D3D12CommandList::Initialize()
{
	CreateCommandList(GetParent());
}

void D3D12CommandList::SetRenderTargets(RHIRenderTargetInfo* InRenderTargets, unsigned int InNumRenderTarget, RHIResource* InDepthStencil)
{
    D3D12RenderTargetView* renderTargetView = D3D12RHI::Cast(InRenderTargets);
    //D3D12DepthStencilView* depthStencilView = static_cast<D3D12DepthStencilView*>(InDepthStencil);

	GetStateCache().SetRenderTargets(&renderTargetView, InNumRenderTarget, nullptr/*depthStencilView*/);
}

void D3D12CommandList::SetStreamResource(std::shared_ptr<RHIResource> InVertexBuffer)
{
    std::shared_ptr<D3D12Buffer> vertexBuffer = std::static_pointer_cast<D3D12Buffer>(InVertexBuffer);
    GetStateCache().SetStreamResource(vertexBuffer, 0, vertexBuffer->GetStride(), 0); // TODO : test data
}

void D3D12CommandList::DrawPrimitive(unsigned int InNumVertices, unsigned int InNumInstances, unsigned int InStartIndex, unsigned int InStartInstance)
{
	GetStateCache().IssueCachedResources();
    CommandList->DrawInstanced(InNumVertices, InNumInstances, InStartIndex, InStartInstance);
}

void D3D12CommandList::DrawIndexedInstanced(std::shared_ptr<class RHIResource> InVertexBuffer, unsigned int InNumIndices, unsigned int InNumInstances, unsigned int InStartIndex, int InStartVertex, unsigned int InStartInstance)
{
	GetStateCache().IssueCachedResources();
    CommandList->DrawIndexedInstanced(InNumIndices, InNumInstances, InStartIndex, InStartVertex, InStartInstance);
}

void D3D12CommandList::AddTransition(D3D12Resource* InResource, const D3D12_RESOURCE_STATES& InAfterState)
{
	assert(InResource);
	
	Barriers.emplace_back(std::move(CD3DX12_RESOURCE_BARRIER::Transition(InResource->GetResource(), InResource->GetResourceState(), InAfterState)));

	InResource->SetResourceState(InAfterState);
}

void D3D12CommandList::FlushTransitions()
{
	CommandList->ResourceBarrier(Barriers.size(), Barriers.data());
	
	Barriers.clear();
}

void D3D12CommandList::ClearDepthStencilView(std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> InDescriptorHandle, D3D12_CLEAR_FLAGS ClearFlags, float InDepthValue, UINT8 InStencil, UINT InNumRects, const D3D12_RECT* InRect/* = nullptr*/)
{
	CommandList->ClearDepthStencilView(InDescriptorHandle.value(), ClearFlags, InDepthValue, InStencil, InNumRects, InRect);
}

void D3D12CommandList::AddDescriptorHeap(D3D12Descriptor* InDescriptor)
{
	assert(InDescriptor);

	Heaps.emplace_back(InDescriptor->Get());
}

void D3D12CommandList::ExecuteHeaps()
{
	if (!Heaps.empty())
		CommandList->SetDescriptorHeaps((UINT)Heaps.size(), &(*Heaps.begin()));
}

void D3D12CommandList::FlushHeaps()
{
	Heaps.clear();
}

void D3D12CommandList::CreateCommandList(D3D12Device* InDevice)
{
	CommandListAllocator = new D3D12CommandAllocator(InDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);

	ThrowIfFailed(InDevice->GetDevice()->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		CommandListAllocator->Get(), // Associated command allocator
		nullptr,                   // Initial PipelineStateObject
		IID_PPV_ARGS(&CommandList)));

	// Start off in a closed state.  This is because the first time we refer 
	// to the command list we will Reset it, and it needs to be closed before
	// calling Reset.
	CommandList->Close();
}