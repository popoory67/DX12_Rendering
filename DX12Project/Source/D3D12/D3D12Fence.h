#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include "MathHelper.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class D3D12Fence
{
public:
	D3D12Fence() = delete;
	D3D12Fence(class D3D12Device* InDevice);
	virtual ~D3D12Fence();

	ID3D12Fence* Get() { return Fence.Get(); }

	void OnEventCompletion(UINT64 InFence);
	void SetFenceCount(UINT64 InFence) { FenceCount = InFence; }

private:
	ComPtr<ID3D12Fence> Fence;
	UINT64 FenceCount = 0;
};