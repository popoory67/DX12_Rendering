#include "D3D12Resource.h"
#include "D3D12Device.h"
#include "D3D12Descriptor.h"
#include "D3DUtil.h"
#include "DDSTextureLoader.h"
#include "Texture.h"

D3D12Resource::D3D12Resource(ID3D12Resource* InResource)
	: Resource(InResource)
	, ResourceState(static_cast<D3D12_RESOURCE_STATES>(-1))
{
	Desc = InResource->GetDesc();
}

D3D12Resource::~D3D12Resource()
{ 
	Reset();
}

void D3D12Resource::Reset()
{
	Resource.Reset();
}

void* D3D12Resource::Lock()
{
	void* resultBuffer;
    CD3DX12_RANGE readRange(0, 0);

    ThrowIfFailed(Resource->Map(0, &readRange, reinterpret_cast<void**>(&resultBuffer)));

	resultBuffer = new char(Size);

	return resultBuffer;
}

void D3D12Resource::Unlock()
{
	Resource->Unmap(0, nullptr);
}

D3D12ResourceLocation::D3D12ResourceLocation(D3D12Resource* InResource)
{
	SetResource(InResource);
}

void D3D12ResourceLocation::SetResource(D3D12Resource* InResource)
{
    assert(InResource);

	Resource = InResource;
}

D3D12ShaderResource::D3D12ShaderResource(ID3D12Resource* InResource)
	: Parent(InResource)
{
	ResourceLocation = std::make_unique<D3D12ResourceLocation>(this);
}

D3D12Buffer::D3D12Buffer(ID3D12Resource* InResource, unsigned int InSize, unsigned int InStride)
	: Parent(InResource)
{
	Size = InSize;
	Stride = InStride;
}
