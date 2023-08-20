#include "D3D12Commands.h"
#include "D3D12Device.h"
#include "D3D12Fence.h"
#include "D3D12Descriptor.h"
#include "D3D12Resource.h"
#include "D3D12PipelineState.h"
#include "D3D12RootSignature.h"
#include "D3DUtil.h"
#include <optional>

D3D12CommandList::D3D12CommandList(D3D12Device* InDevice)
	: D3D12Api(InDevice)
{

}

D3D12CommandList::~D3D12CommandList()
{
    SafeDelete(CommandListAllocator);
    SafeDelete(Fence);
}

void D3D12CommandList::Reset()
{
	// GPU가 command list의 명령을 모두 처리한 후에 리셋
	CommandListAllocator->Reset();

	// 이전에 Excute하여 Queue에 command list를 추가했으니 list를 재설정해도 된다.
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

void D3D12CommandList::Initialize()
{
	CreateCommandList(GetParent());
}

void D3D12CommandList::SetStreamResource(std::shared_ptr<RHIResource> InVertexBuffer) const
{
    std::shared_ptr<D3D12Buffer> vertexBuffer = std::static_pointer_cast<D3D12Buffer>(InVertexBuffer);
    GetParent()->GetPSOCache().SetStreamResource(vertexBuffer, 0, vertexBuffer->GetStride(), 0); // TODO : test data
}

void D3D12CommandList::DrawIndexedInstanced(std::shared_ptr<class RHIResource> InVertexBuffer, unsigned int InIndexCount, unsigned int InInstanceCount, unsigned int InStartIndex, int InBaseVertexIndex, unsigned int InStartInstance) const
{
	// TODO : test
	{
        std::shared_ptr<D3D12Buffer> vertexBuffer = std::static_pointer_cast<D3D12Buffer>(InVertexBuffer);

        D3D12_VERTEX_BUFFER_VIEW view;
        view.BufferLocation = vertexBuffer ? vertexBuffer->GetGPUVirtualAddress() : 0;
        view.StrideInBytes = 28;
        view.SizeInBytes = vertexBuffer ? vertexBuffer->GetSize() : 0;

        CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        CommandList->IASetVertexBuffers(0, 1, &view);
    }
    CommandList->DrawIndexedInstanced(InIndexCount, InInstanceCount, InStartIndex, InBaseVertexIndex, InStartInstance);
}

void D3D12CommandList::AddTransition(std::shared_ptr<D3D12Resource> InResource, const D3D12_RESOURCE_STATES& InAfterState)
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

void D3D12CommandList::ClearRenderTargetView(std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> InDescriptorHandle, XMVECTORF32 InBackColor, UINT InNumRects, const D3D12_RECT* InRect/* = nullptr*/)
{
	CommandList->ClearRenderTargetView(InDescriptorHandle.value(), InBackColor, InNumRects, InRect);
}

void D3D12CommandList::ClearDepthStencilView(std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> InDescriptorHandle, D3D12_CLEAR_FLAGS ClearFlags, float InDepthValue, UINT8 InStencil, UINT InNumRects, const D3D12_RECT* InRect/* = nullptr*/)
{
	CommandList->ClearDepthStencilView(InDescriptorHandle.value(), ClearFlags, InDepthValue, InStencil, InNumRects, InRect);
}

void D3D12CommandList::SetRenderTargets(UINT InNumRenderTargetDescriptors, std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> InRenderTargetDescriptorHandle, bool InSingleHandleToDescriptorRange, std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> InDepthStencilDescriptorHandle)
{
	if (InDepthStencilDescriptorHandle.has_value())
	{
		CommandList->OMSetRenderTargets(InNumRenderTargetDescriptors, &InRenderTargetDescriptorHandle.value(),
			InSingleHandleToDescriptorRange, &InDepthStencilDescriptorHandle.value());
	}
	else
	{
		CommandList->OMSetRenderTargets(InNumRenderTargetDescriptors, &InRenderTargetDescriptorHandle.value(),
			InSingleHandleToDescriptorRange, nullptr);
	}
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