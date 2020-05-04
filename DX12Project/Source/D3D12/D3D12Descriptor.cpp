#include "stdafx.h"
#include "D3D12Descriptor.h"

D3D12Descriptor::D3D12Descriptor(D3D12DeviceChild* InDeivce, D3D12_DESCRIPTOR_HEAP_DESC& InHeapDesc)
	: D3D12DeviceChild(*InDeivce)
{
	GetParent()->CreateDescriptorHeap(InHeapDesc, Heap);

	Size = GetParent()->GetDescriptorHandleIncrementSize(InHeapDesc.Type);
	Count = InHeapDesc.NumDescriptors;
}

D3D12Descriptor::D3D12Descriptor(D3D12DeviceChild* InDeivce, D3D12_DESCRIPTOR_HEAP_TYPE InType, D3D12_DESCRIPTOR_HEAP_FLAGS InFlags, UINT64 InCount)
	: D3D12DeviceChild(*InDeivce)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Flags = InFlags;
	desc.NumDescriptors = (UINT)InCount;
	desc.Type = InType;

	GetParent()->CreateDescriptorHeap(desc, Heap);

	Size = GetParent()->GetDescriptorHandleIncrementSize(desc.Type);
	Count = InCount;
}

D3D12_GPU_DESCRIPTOR_HANDLE D3D12Descriptor::GetGpuHandle(UINT64 InIndex) const
{
	assert(Heap);

	if (InIndex >= Count)
	{
		throw std::out_of_range("D3DX12_GPU_DESCRIPTOR_HANDLE");
	}

	D3D12_GPU_DESCRIPTOR_HANDLE handle;
	handle.ptr = GPUHandle.ptr + InIndex * Size;
	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12Descriptor::GetCpuHandle(UINT64 InIndex) const
{
	assert(Heap);

	if (InIndex >= Count)
	{
		throw std::out_of_range("D3DX12_CPU_DESCRIPTOR_HANDLE");
	}

	D3D12_CPU_DESCRIPTOR_HANDLE handle;
	handle.ptr = static_cast<SIZE_T>(CPUHandle.ptr + InIndex * Size);
	return handle;
}