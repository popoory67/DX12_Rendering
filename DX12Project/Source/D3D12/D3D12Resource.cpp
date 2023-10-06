#include "D3D12Resource.h"
#include "D3D12Device.h"
#include "D3DUtil.h"
#include "DDSTextureLoader.h"
#include "Texture.h"

D3D12Resource::D3D12Resource(ComPtr<ID3D12Resource>&& InResource, unsigned int InSize/* = 0*/, unsigned int InStride/* = 0*/)
	: Resource(std::move(InResource))
	, ResourceState(static_cast<D3D12_RESOURCE_STATES>(-1))
{
	Desc = Resource->GetDesc();
	Size = InSize;
	Stride = InStride;
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

    ThrowIfFailed(Resource->Map(0, &readRange, &resultBuffer));

	return resultBuffer;
}

void D3D12Resource::Unlock()
{
	Resource->Unmap(0, nullptr);
}

D3D12Buffer::D3D12Buffer(ComPtr<ID3D12Resource>&& InResource, unsigned int InSize, unsigned int InStride)
    : Parent(std::move(InResource), InSize, InStride)
{

}

D3D12Buffer::~D3D12Buffer()
{

}