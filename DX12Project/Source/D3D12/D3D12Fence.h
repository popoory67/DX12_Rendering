#pragma once

#include <d3d12.h>
#include "D3D12Device.h"

// This is an interface for a GPU/CPU synchronization object.
// The fence object stores a single 64-bit unsigned integer value that can be used to know the GPU has finished its job on a command queue.
class D3D12Fence : public D3D12Api
{
public:
	D3D12Fence() = delete;
	explicit D3D12Fence(class D3D12Device* InDevice, UINT64 InInitialValue = 0);
	virtual ~D3D12Fence();

	ID3D12Fence* Get() { return Fence.Get(); }

	// Stall CPU thread until the fence value has reached what we have specified as ID3D12CommandQueue::Signal.
	void CpuWait(UINT64 InFenceValue);

	// Until the fence value reaches a predetermined value that is set by ID3D12Fence::Signal, the GPU will be paused.
	void GpuWait(UINT64 InFenceValue);

	// Set a fence value into the command queue.
	// The fence object will be set the completed value when the value will be accomplished in the command queue.
	UINT64 Signal();

private:
	void SignalInternal(UINT64 InFenceValue);
	bool IsFenceCompleted(UINT64 InFenceValue);

private:
	ComPtr<ID3D12Fence> Fence;
	UINT64 CurrentFence;
#if _DEBUG
	UINT64 DebugFence;
#endif

	HANDLE CompleteEventHandle;
};

//class D3D12FencePool
//{
//public:
//
//};