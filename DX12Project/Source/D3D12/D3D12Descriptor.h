#pragma once
#include <d3d12.h>
#include "MathHelper.h"
#include "D3D12Device.h"

class D3D12Descriptor : public D3D12Api
{
public:
	D3D12Descriptor() = delete;
	D3D12Descriptor(D3D12Device* InDeivce, D3D12_DESCRIPTOR_HEAP_DESC& InHeapDesc);
	D3D12Descriptor(D3D12Device* InDeivce, D3D12_DESCRIPTOR_HEAP_TYPE InType, D3D12_DESCRIPTOR_HEAP_FLAGS InFlags, UINT64 InCount);
	virtual ~D3D12Descriptor() {}

	FORCEINLINE UINT64 GetSize() const { return Size; }
	FORCEINLINE void SetSize(UINT64 InSize) { Size = InSize; }
	FORCEINLINE UINT64 GetCount() const { return Count; }

	ComPtr<ID3D12DescriptorHeap>& GetHeap() { return Heap; }
	ID3D12DescriptorHeap* Get() { return Heap.Get(); }

	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(UINT64 InIndex) const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(UINT64 InIndex) const;

private:
	UINT64 Size;
	UINT64 Count;

	D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle;

	ComPtr<ID3D12DescriptorHeap> Heap = nullptr;
};

class D3D12DescriptorCache : public D3D12Api, public std::enable_shared_from_this<D3D12DescriptorCache>
{
public:
	D3D12DescriptorCache() = delete;
	D3D12DescriptorCache(D3D12Device* InDeivce);
	virtual ~D3D12DescriptorCache() = default;

	void SetVertexBuffers(struct D3D12VertexBufferCache& InCache);
	void SetRenderTargets(class D3D12RenderTargetView** InRenderTargets, unsigned int InNumRenderTargets, class D3D12DepthStencilView* InDepthStencil);
};