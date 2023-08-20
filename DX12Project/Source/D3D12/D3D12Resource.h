#pragma once
#include "D3D12Device.h"
#include "RenderInterface.h"
#include <memory>

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

class D3D12Resource : public RHIResource, std::enable_shared_from_this<D3D12Resource>
{
public:
	D3D12Resource() = delete;
	D3D12Resource(ID3D12Resource* InResource);
	virtual ~D3D12Resource();

	ComPtr<ID3D12Resource>& Get() noexcept { return Resource; }
	ID3D12Resource* GetResource() noexcept { return Resource.Get(); }
	
	FORCEINLINE D3D12_RESOURCE_DESC GetDesc() const noexcept { return Desc; }
	FORCEINLINE D3D12_RESOURCE_STATES GetResourceState() const noexcept { return ResourceState; }

	FORCEINLINE void SetResourceState(const D3D12_RESOURCE_STATES& InNewState) noexcept
	{
		ResourceState = InNewState; 
	}

	void Reset();

	void* Lock() override;
	void Unlock() override;

protected:
	ComPtr<ID3D12Resource> Resource = nullptr;
	
	D3D12ResourceDesc Desc;
	D3D12_RESOURCE_STATES ResourceState;
};

class D3D12ResourceLocation
{
public:
	D3D12ResourceLocation() = delete;
	explicit D3D12ResourceLocation(D3D12Resource* InResource);
	virtual ~D3D12ResourceLocation() = default;

	void SetResource(D3D12Resource* InResource);

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const noexcept
	{
		return Resource->Get()->GetGPUVirtualAddress(); 
	}

private:
	D3D12Resource* Resource = nullptr;
};

class D3D12ShaderResource : public D3D12Resource
{
	using Parent = D3D12Resource;

public:
	D3D12ShaderResource() = delete;
	explicit D3D12ShaderResource(ID3D12Resource* InResource);
	virtual ~D3D12ShaderResource() = default;

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const noexcept { return ResourceLocation->GetGPUVirtualAddress(); }
	
private:
	std::unique_ptr<D3D12ResourceLocation> ResourceLocation;
};

class D3D12Buffer : public D3D12ShaderResource
{
	using Parent = D3D12ShaderResource;

public:
	D3D12Buffer() = delete;
	explicit D3D12Buffer(ID3D12Resource* InResource, unsigned int InSize, unsigned int InStride);
	virtual ~D3D12Buffer() = default;
};