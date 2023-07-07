#include "stdafx.h"
#include "D3D12Device.h"
#include "D3D12Commands.h"
#include "D3D12Descriptor.h"
#include "D3D12Resource.h"
#include "D3D12PipelineState.h"
#include "D3D12RootSignature.h"

D3D12CommandList::D3D12CommandList(D3D12Device* InDevice)
	: D3D12Api(InDevice)
{

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

void D3D12CommandList::SetPipelineState(std::shared_ptr<class D3D12PipelineState> InPipelineState) const
{
	CommandList->SetPipelineState(InPipelineState->GetInterface());
}

void D3D12CommandList::SetVertexBuffers(D3D12VertexBufferCache& InVertexBufferCache) const
{
	// TODO : 이걸 어디서 호출해주지 => 어떤 것을 그리려고 준비하는 시점에 호출, 즉 컨텐츠 구현같은거할때 = 구현하는 사람이 데이터 넣고(사용자) 그리는건 엔진에서 알아서
	// D3D12VertexBuffer* InVertexBuffer, uint32_t StreamIndex, uint32_t InStride, uint32_t InOffset
//	StateCache.SetStreamSource(InVertexBuffer, StreamIndex, InStride, InOffset);
	
	CommandList->IASetVertexBuffers(0, InVertexBufferCache.MaxVertexIndex + 1, InVertexBufferCache.CurrentVertexBufferView);
}

void D3D12CommandList::SetIndexBuffer(std::optional<D3D12_INDEX_BUFFER_VIEW> InView/* = {}*/) const
{
	CommandList->IASetIndexBuffer(&InView.value());
}

void D3D12CommandList::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY InPrimitiveTopology) const
{
	CommandList->IASetPrimitiveTopology(InPrimitiveTopology);
}

void D3D12CommandList::DrawIndexedInstanced(UINT InIndexCountPerInstance, UINT InInstanceCount, UINT InStartIndexLocation, INT InBaseVertexLocation, UINT InStartInstanceLocation) const
{
	CommandList->DrawIndexedInstanced(InIndexCountPerInstance, InInstanceCount, InStartIndexLocation, InBaseVertexLocation, InStartInstanceLocation);
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

void D3D12CommandList::SetRootSignature(D3D12RootSignature* InRootSignature)
{
	assert(InRootSignature);

	CommandList->SetGraphicsRootSignature(InRootSignature->GetInterface());
}

void D3D12CommandList::BindTable(RenderType InRenderType, CD3DX12_GPU_DESCRIPTOR_HANDLE InDescriptorHandle)
{
/*	InDescriptorHandle.Offset(mSkyTexHeapIndex, mCbvSrvUavDescriptorSize);*/
	CommandList->SetGraphicsRootDescriptorTable((UINT)InRenderType, InDescriptorHandle);
}

void D3D12CommandList::BindConstBuffer(RenderType InRenderType, std::vector<D3D12_GPU_VIRTUAL_ADDRESS>& InAddresses)
{
	for (D3D12_GPU_VIRTUAL_ADDRESS address : InAddresses)
		CommandList->SetGraphicsRootConstantBufferView((UINT)InRenderType, address);
}

void D3D12CommandList::BindConstBuffer(RenderType InRenderType, D3D12_GPU_VIRTUAL_ADDRESS& InAddress)
{
	CommandList->SetGraphicsRootConstantBufferView((UINT)InRenderType, InAddress);
}

void D3D12CommandList::BindShaderResource(RenderType InRenderType, std::vector<D3D12_GPU_VIRTUAL_ADDRESS>& InAddresses)
{
	for (D3D12_GPU_VIRTUAL_ADDRESS address : InAddresses)
		CommandList->SetGraphicsRootShaderResourceView((UINT)InRenderType, address);
}

void D3D12CommandList::BindShaderResource(RenderType InRenderType, D3D12_GPU_VIRTUAL_ADDRESS& InAddress)
{
	CommandList->SetGraphicsRootShaderResourceView((UINT)InRenderType, InAddress);
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