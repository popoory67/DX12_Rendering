#include "D3D12Device.h"
#include "D3D12RenderInterface.h"
#include "D3D12Resource.h"
#include "D3D12Descriptor.h"
#include "D3D12PipelineState.h"
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

RHIResource* D3D12RHI::CreateVertexBuffer(unsigned int InSize, unsigned int InStride)
{
	ComPtr<ID3D12Resource> resource = CreateResource(InSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
    return new D3D12Buffer(std::move(resource), InSize, InStride);
}

void* D3D12RHI::LockBuffer(RHIResource* InBuffer)
{
	return InBuffer->Lock();
}

void D3D12RHI::UnlockBuffer(RHIResource* InBuffer)
{
	InBuffer->Unlock();
}

ComPtr<ID3D12Resource> D3D12RHI::CreateResource(unsigned int InByteSize, const D3D12_HEAP_TYPE InHeapType, const D3D12_RESOURCE_STATES InResourceState, const D3D12_CLEAR_VALUE* InValue)
{
	ComPtr<ID3D12Resource> newResource;
    D3D12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(InHeapType);
	D3D12_RESOURCE_DESC desc{ CD3DX12_RESOURCE_DESC::Buffer(InByteSize) };

    ThrowIfFailed(GetCurrentDevice()->GetDevice()->CreateCommittedResource(
        &properties,
        D3D12_HEAP_FLAG_NONE,
        &desc,
		InResourceState,
        InValue,
        IID_PPV_ARGS(&newResource)));

	return newResource;
}

void D3D12RHI::CreateRenderTarget(D3D12Resource* InResource, CD3DX12_CPU_DESCRIPTOR_HANDLE& InDescriptorHandle, UINT InDescriptorSize)
{
	assert(InResource);

	GetCurrentDevice()->GetDevice()->CreateRenderTargetView(InResource->GetResource(), nullptr, InDescriptorHandle);

	InDescriptorHandle.Offset(1, InDescriptorSize);
}

//void D3D12RHI::CreateShaderResource(D3D12ShaderResource* InResource, D3D12Descriptor* InDescriptor, std::string InName/* = nullptr*/, std::wstring InFilePath/* = nullptr*/)
//{
//	assert(InDescriptor);
//	assert(InResource);
//
//	// Create the SRV heap.
//	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
//	srvHeapDesc.NumDescriptors = 1;
//	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//
//	InDescriptor = new D3D12Descriptor(CurrentDevice, srvHeapDesc);
//
//	LoadTexture(InResource, InName, InFilePath);
//
//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//	srvDesc.Format = InResource->GetResource()->GetDesc().Format;
//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
//	srvDesc.Texture2D.MostDetailedMip = 0;
//	srvDesc.Texture2D.MipLevels = InResource->GetResource()->GetDesc().MipLevels;
//	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
//
//	GetCurrentDevice()->GetDevice()->CreateShaderResourceView(InResource->GetResource()->GetResource(), &srvDesc, InDescriptor->GetCpuHandle(0));
//}

//void D3D12RHI::LoadTexture(D3D12ShaderResource* InResource, std::string InName, std::wstring InFilePath)
//{
//	if (InResource)
//	{
//		//InTexture->Name = InName; // "woodCrateTex";
//		//InTexture->Filename = InFilePath;// L"../../Textures/WoodCrate01.dds";
//
//		//ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(GetParent()->GetDeviceInterface(), GetCommnadList()->GetGraphicsInterface(), InFilePath.c_str(), InResource->Get(), InResource->GetTextureResource()->Get()));
//	}
//}

void D3D12RHI::CreateDepthStencilView(D3D12Resource* InResource, class D3D12Descriptor* InDescriptor, D3D12_DEPTH_STENCIL_VIEW_DESC& InDepthStencilDesc)
{
	assert(InDescriptor);
	assert(InResource);

	GetCurrentDevice()->GetDevice()->CreateDepthStencilView(InResource->GetResource(), &InDepthStencilDesc, InDescriptor->GetCpuHandle(0));
}