#pragma once
#include "D3D12Types.h"
#include "D3D12Device.h"
#include "RenderInterface.h"
#include <memory>
#include <d3d12.h>

struct D3D12ResourceDesc : public D3D12_RESOURCE_DESC
{
	D3D12ResourceDesc() = default;
	D3D12ResourceDesc(const CD3DX12_RESOURCE_DESC& InDesc)
		: D3D12_RESOURCE_DESC(InDesc)
	{
	}

	D3D12ResourceDesc(const D3D12_RESOURCE_DESC& InDesc)
		: D3D12_RESOURCE_DESC(InDesc)
	{
	}

};

class D3D12Resource : public RHIResource
{
public:
	D3D12Resource() = delete;
	D3D12Resource(ComPtr<ID3D12Resource>&& InResource, unsigned int InSize = 0, unsigned int InStride = 0);
	virtual ~D3D12Resource();

	ComPtr<ID3D12Resource>& Get() noexcept { return Resource; }
	ID3D12Resource* GetResource() noexcept { return Resource.Get(); }
	
	FORCEINLINE D3D12_RESOURCE_DESC GetDesc() const noexcept { return Desc; }
	FORCEINLINE D3D12_RESOURCE_STATES GetResourceState() const noexcept { return ResourceState; }

	FORCEINLINE void SetResourceState(const D3D12_RESOURCE_STATES& InNewState) noexcept
	{
		ResourceState = InNewState; 
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;

	void Reset() override;

	void* Lock() override;
	void Unlock() override;

	ComPtr<ID3D12Resource> operator->()
	{
		return Resource;
	}

protected:
	ComPtr<ID3D12Resource> Resource = nullptr;
	
	D3D12ResourceDesc Desc;
	D3D12_RESOURCE_STATES ResourceState;
};

class D3D12Buffer : public D3D12Resource
{
	using Parent = D3D12Resource;

public:
	D3D12Buffer() = delete;
	explicit D3D12Buffer(ComPtr<ID3D12Resource>&& InResource, unsigned int InSize, unsigned int InStride);
	virtual ~D3D12Buffer();
};

template<>
struct TD3D12Types<RHIResource>
{
	using ResourceType = D3D12Resource;
};