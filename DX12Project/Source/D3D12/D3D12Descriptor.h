#pragma once

#include <d3d12.h>
#include "MathHelper.h"
#include "D3D12Device.h"

class D3D12Descriptor : public D3D12DeviceChild
{
public:
	D3D12Descriptor() = delete;
	D3D12Descriptor(class D3D12DeviceChild* InDeivce, D3D12_DESCRIPTOR_HEAP_DESC& InHeapDesc);
	D3D12Descriptor(class D3D12DeviceChild* InDeivce, D3D12_DESCRIPTOR_HEAP_TYPE InType, D3D12_DESCRIPTOR_HEAP_FLAGS InFlags, UINT64 InCount);
	virtual ~D3D12Descriptor() {}

	UINT64 GetSize() { return Size; }
	void SetSize(UINT64 InSize) { Size = InSize; }
	UINT64 GetCount() { return Count; }

	ComPtr<ID3D12DescriptorHeap>& GetHeap() { return Heap; }
	ID3D12DescriptorHeap* Get() { return Heap.Get(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() { return Heap->GetCPUDescriptorHandleForHeapStart(); }

	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(UINT64 InIndex) const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(UINT64 InIndex) const;

private:
	UINT64 Size = 0;
	UINT64 Count = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle;

	ComPtr<ID3D12DescriptorHeap> Heap = nullptr;
};
