#include "D3D12Device.h"
#include "D3D12RenderInterface.h"
#include "D3D12Resource.h"
#include "D3D12Descriptor.h"
#include "D3D12Commands.h"

D3D12RHI::D3D12RHI()
	: CurrentDevice(nullptr)
{

}

D3D12RHI::~D3D12RHI()
{
	Destroy();
}

void D3D12RHI::Initialize()
{
	if (!CurrentDevice)
	{
		CurrentDevice = new D3D12Device();
		CurrentDevice->Initialize();
	}
}

void D3D12RHI::Destroy()
{
	SafeDelete(CurrentDevice);
}

RHIResource* D3D12RHI::CreateBuffer(unsigned int InSize, unsigned int InStride)
{
    ComPtr<ID3D12Resource> resource = CreateResource(InSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
    return new D3D12Buffer(std::move(resource), InSize, InStride);
}

RHIResource* D3D12RHI::CreateVertexBuffer(unsigned int InVertexSize, unsigned int InIndexSize, unsigned int InStride)
{
	// D3D12_HEAP_TYPE_UPLOAD : to copy the resource CPU to GPU
	ComPtr<ID3D12Resource> resource = CreateResource(InVertexSize + InIndexSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
    return new D3D12Buffer(std::move(resource), InVertexSize, InStride);
}

UINT8* D3D12RHI::LockBuffer(RHIResource* InBuffer)
{
	return static_cast<UINT8*>(InBuffer->Lock());
}

void D3D12RHI::UnlockBuffer(RHIResource* InBuffer)
{
	InBuffer->Unlock();
}

RHIResource* D3D12RHI::CreateTexture(TextureSettings* InTextureSettings)
{
    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = InTextureSettings->MipLevel;
    textureDesc.Format = static_cast<DXGI_FORMAT>(InTextureSettings->Format);
    textureDesc.Width = InTextureSettings->Width;
    textureDesc.Height = InTextureSettings->Height;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    ComPtr<ID3D12Resource> textureResource = CreateResource(textureDesc, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST, nullptr);

    unsigned int size = textureDesc.Width * textureDesc.Height * InTextureSettings->Channels;
    return new D3D12Resource(std::move(textureResource), size);
}

void D3D12RHI::SetShaderResource(RHIResource* InResource, unsigned int InFormat)
{
    D3D12Resource* resource = D3D12RHI::Cast(InResource);
    assert(resource);

    // Descriptor
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 1;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    D3D12Descriptor* descriptor = new D3D12Descriptor(GetCurrentDevice(), srvHeapDesc);
    {
        GetCurrentDevice()->GetResourceManager().AddDescriptorHeap(descriptor);
    }

    // Create a SRV for the texture.
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = static_cast<DXGI_FORMAT>(InFormat);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    GetCurrentDevice()->GetDevice()->CreateShaderResourceView(
        resource->GetResource(),
        &srvDesc,
        descriptor->GetCpuHandle(0));
}

ComPtr<ID3D12Resource> D3D12RHI::CreateResource(unsigned int InByteSize, const D3D12_HEAP_TYPE InHeapType, const D3D12_RESOURCE_STATES InResourceState, const D3D12_CLEAR_VALUE* InValue)
{
	ComPtr<ID3D12Resource> newResource;
    D3D12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(InHeapType);
	D3D12_RESOURCE_DESC desc{ CD3DX12_RESOURCE_DESC::Buffer(InByteSize) }; // This is not a texture, but a buffer setting.

    ThrowIfFailed(GetCurrentDevice()->GetDevice()->CreateCommittedResource(
        &properties,
        D3D12_HEAP_FLAG_NONE,
        &desc,
		InResourceState,
        InValue,
        IID_PPV_ARGS(&newResource)));

	return newResource;
}

ComPtr<ID3D12Resource> D3D12RHI::CreateResource(D3D12_RESOURCE_DESC InDesc, const D3D12_HEAP_TYPE InHeapType, const D3D12_RESOURCE_STATES InResourceState, const D3D12_CLEAR_VALUE* InValue)
{
    ComPtr<ID3D12Resource> newResource;
    D3D12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(InHeapType);

    ThrowIfFailed(GetCurrentDevice()->GetDevice()->CreateCommittedResource(
        &properties,
        D3D12_HEAP_FLAG_NONE,
        &InDesc,
        InResourceState,
        InValue,
        IID_PPV_ARGS(&newResource)));

    return newResource;
}