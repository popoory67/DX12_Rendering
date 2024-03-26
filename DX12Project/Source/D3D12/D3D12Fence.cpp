#include "D3D12Fence.h"
#include "D3DUtil.h"

D3D12Fence::D3D12Fence(D3D12Device* InDevice, UINT64 InInitialValue)
	: D3D12Api(InDevice)
	, CurrentFence(InInitialValue)
#if _DEBUG
	, DebugFence(InInitialValue)
#endif
{
	assert(InDevice);

	CompleteEventHandle = CreateEventExW(nullptr, L"", false, EVENT_ALL_ACCESS);
	assert(CompleteEventHandle != INVALID_HANDLE_VALUE);

	ThrowIfFailed(InDevice->GetDevice()->CreateFence(InInitialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));

	if (CurrentFence == 0)
	{
		CurrentFence = Fence->GetCompletedValue();
#if _DEBUG
		DebugFence = Fence->GetCompletedValue();
#endif
	}
}

D3D12Fence::~D3D12Fence()
{
	if (CompleteEventHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(CompleteEventHandle);
		CompleteEventHandle = INVALID_HANDLE_VALUE;
	}
}

void D3D12Fence::CpuWait(UINT64 InFenceValue)
{
	if (!IsFenceCompleted(InFenceValue))
	{
		// CPU is waiting for the fence value to be InFenceValue.
		ThrowIfFailed(Fence->SetEventOnCompletion(InFenceValue, CompleteEventHandle));
		const DWORD result = WaitForSingleObject(CompleteEventHandle, INFINITE);
		assert(result == 0);
	}
}

void D3D12Fence::GpuWait(UINT64 InFenceValue)
{
	if (ID3D12CommandQueue* commandQueue = GetParent()->GetCommandQueue())
	{
		commandQueue->Wait(Fence.Get(), InFenceValue);
	}
}

UINT64 D3D12Fence::Signal()
{
	++CurrentFence;
	SignalInternal(CurrentFence);

	// Advance the fence value to mark commands up to this fence point.
	//CurrentFence = Fence->GetCompletedValue() + 1;

	return CurrentFence;
}

void D3D12Fence::SignalInternal(UINT64 InFenceValue)
{
	if (ID3D12CommandQueue* commandQueue = GetParent()->GetCommandQueue())
	{
		// ID3D12CommandQueue::Signal is a command that increase a fence value
		// after the previous GPU commands is executed.
		commandQueue->Signal(Fence.Get(), InFenceValue);
#if _DEBUG
		DebugFence = Fence->GetCompletedValue();
#endif
	}
}

bool D3D12Fence::IsFenceCompleted(UINT64 InFenceValue)
{
	return InFenceValue <= Fence->GetCompletedValue();
}
