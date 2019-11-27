#include "stdafx.h"
#include "D3D12Fence.h"
#include "D3D12Device.h"
#include "D3DUtil.h"

D3D12Fence::D3D12Fence(D3D12Device* InDevice)
{
	assert(InDevice || InDevice->GetDevice());

	ThrowIfFailed(InDevice->GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));
}

D3D12Fence::~D3D12Fence()
{

}

void D3D12Fence::OnEventCompletion(UINT64 InFence)
{
	HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

	ThrowIfFailed(Fence->SetEventOnCompletion(InFence, eventHandle));
	WaitForSingleObject(eventHandle, INFINITE);
	CloseHandle(eventHandle);
}