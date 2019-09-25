#include "stdafx.h"
#include "D3D12Commands.h"
#include "D3D12Device.h"

D3D12CommandList::D3D12CommandList(ComPtr<ID3D12GraphicsCommandList>& InCommandList, ComPtr<ID3D12Device>& InD3D12Device)
	: CommandList(InCommandList)
{
	if (CommandList.Get())
	{
		if (InD3D12Device.Get())
		{
			ThrowIfFailed(InD3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CommandListAllocator.GetAddressOf())));
			ThrowIfFailed(InD3D12Device->CreateCommandList(
				0,
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				CommandListAllocator.Get(), // Associated command allocator
				nullptr,                   // Initial PipelineStateObject
				IID_PPV_ARGS(CommandList.GetAddressOf())));

			// Start off in a closed state.  This is because the first time we refer 
			// to the command list we will Reset it, and it needs to be closed before
			// calling Reset.
			CommandList->Close();
		}
	}
}

D3D12CommandListExecutor::D3D12CommandListExecutor(D3D12Device* InD3D12Device)
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	ThrowIfFailed(InD3D12Device->GetDevice()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&CommandQueue)));
}

void D3D12CommandListExecutor::Execute(CommandListBase& InCommandList)
{
	D3D12CommandList* pCmdList = static_cast<D3D12CommandList*>(&InCommandList);
	if (pCmdList)
	{
		ComPtr<ID3D12GraphicsCommandList> d3d12CommandList = pCmdList->Get();

		ThrowIfFailed(d3d12CommandList->Close());

		ID3D12CommandList* cmdsLists[] = { d3d12CommandList.Get() };
		CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	}
}

void D3D12CommandListExecutor::FlushCommands()
{
	// Advance the fence value to mark commands up to this fence point.
	CurrentFenceCount++;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	ThrowIfFailed(CommandQueue->Signal(Fence.Get(), CurrentFenceCount));

	// Wait until the GPU has completed commands up to this fence point.
	if (Fence->GetCompletedValue() < CurrentFenceCount)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.  
		ThrowIfFailed(Fence->SetEventOnCompletion(CurrentFenceCount, eventHandle));

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}