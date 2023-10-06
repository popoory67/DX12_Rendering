#include "D3D12Commands.h"
#include "D3D12Device.h"
#include "D3D12Fence.h"
#include "D3D12RenderInterface.h"


D3D12CommandListExecutor::D3D12CommandListExecutor(D3D12Device* InDevice)
	: D3D12Api(InDevice)
{

}

D3D12CommandListExecutor::~D3D12CommandListExecutor()
{
	CommandQueue.Reset();
	SafeDelete(Fence);
}

void D3D12CommandListExecutor::Initialize()
{
	Fence = new D3D12Fence(GetParent());

	CreateCommandQueue(GetParent());
}

void D3D12CommandListExecutor::ExecuteCommandLists(RHICommandList* InCommandList)
{
	D3D12CommandList* commandList = D3D12RHI::Cast(InCommandList);
	
	// TODO
	// Instead of managing one command list, the command list must be managed as an array.
	ID3D12CommandList* cmdsLists[] = { commandList->GetCommandLists() };
	CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
}

void D3D12CommandListExecutor::FlushCommandLists()
{
	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	UINT64 currentFence = Fence->Signal();

	// Wait until the GPU has completed commands up to this fence point.
	Fence->CpuWait(currentFence);
}

void D3D12CommandListExecutor::CreateCommandQueue(D3D12Device* InDevice)
{
	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	ThrowIfFailed(InDevice->GetDevice()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&CommandQueue)));
}