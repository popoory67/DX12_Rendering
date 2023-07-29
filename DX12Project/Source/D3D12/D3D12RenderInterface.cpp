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

}

void D3D12RHI::ResetCommandList()
{
	D3D12CommandList& commandList = CurrentDevice->GetCommandList();
	commandList.Reset();
}

void D3D12RHI::FlushCommandQueue() const
{
	//CurrentDevice->GetCommandListExecutor().FlushCommandLists();
}

void D3D12RHI::CreateResource(D3D12Resource* InResource, D3D12_RESOURCE_DESC& InDesc, D3D12_CLEAR_VALUE& InValue)
{
	ComPtr<ID3D12Resource> newResource;
	D3D12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	ThrowIfFailed(InResource->GetDevice()->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&InDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&InValue,
		IID_PPV_ARGS(&newResource)));

	if (InResource)
	{
		InResource->Reset();
		SafeDelete(InResource);
	}

	InResource = new D3D12Resource(CurrentDevice, newResource.Get());
}

void D3D12RHI::CreateResource(D3D12Resource* InResource, UINT64 InByteSize, D3D12_CLEAR_VALUE& InValue)
{
	D3D12_RESOURCE_DESC desc{ CD3DX12_RESOURCE_DESC::Buffer(InByteSize) };
	CreateResource(InResource, desc, InValue);
}

void D3D12RHI::CreateDefaultBuffer(D3D12DefaultResource* InResource, const void* InInitData, UINT64 InByteSize)
{
	assert(InResource);

	//D3D12Resource* pDefaultResource = new D3D12Resource(CurrentDevice);

	//// Create the actual default buffer resource.
	//CreateResource(InResource, InByteSize, D3D12_CLEAR_VALUE{});

	//// In order to copy CPU memory data into our default buffer, we need to create an intermediate upload heap. 
	//// Create the GPU upload buffer.
	//ThrowIfFailed(InResource->GetDevice()->CreateCommittedResource(
	//	&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
	//	D3D12_HEAP_FLAG_NONE,
	//	&CD3DX12_RESOURCE_DESC::Buffer(InByteSize),
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&textureUploadHeap)));

	//// Describe the data we want to copy into the default buffer.
	//D3D12_SUBRESOURCE_DATA subResourceData = {};
	//subResourceData.pData = InInitData;
	//subResourceData.RowPitch = InByteSize;
	//subResourceData.SlicePitch = subResourceData.RowPitch;

	//// Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
	//// will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
	//// the intermediate upload heap data will be copied to mBuffer.
	//CommandList->ResourceBarrier(pDefaultResource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	//UpdateSubresources<1>(CommandList->GetGraphicsInterface(), pDefaultResource->GetInterface(), InResource->GetInterface(), 0, 0, 1, &subResourceData);
	//CommandList->ResourceBarrier(pDefaultResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);

	// Note: UploadBuffer has to be kept alive after the above function calls because
	// the command list has not been executed yet that performs the actual copy.
	// The caller can Release the UploadBuffer after it knows the copy has been executed.
}

void D3D12RHI::CreateRenderTarget(D3D12Resource* InResource, CD3DX12_CPU_DESCRIPTOR_HANDLE& InDescriptorHandle, UINT InDescriptorSize)
{
	assert(InResource);

	InResource->GetDevice()->CreateRenderTargetView(InResource->GetResource(), nullptr, InDescriptorHandle);

	InDescriptorHandle.Offset(1, InDescriptorSize);
}

void D3D12RHI::CreateShaderResource(D3D12ShaderResource* InResource, D3D12Descriptor* InDescriptor, std::string InName/* = nullptr*/, std::wstring InFilePath/* = nullptr*/)
{
	assert(InDescriptor);
	assert(InResource);

	// Create the SRV heap.
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	InDescriptor = new D3D12Descriptor(CurrentDevice, srvHeapDesc);

	LoadTexture(InResource, InName, InFilePath);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = InResource->GetResource()->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = InResource->GetResource()->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	InResource->GetDevice()->CreateShaderResourceView(InResource->GetResource()->GetResource(), &srvDesc, InDescriptor->GetCpuHandle(0));
}

void D3D12RHI::LoadTexture(D3D12ShaderResource* InResource, std::string InName, std::wstring InFilePath)
{
	if (InResource)
	{
		//InTexture->Name = InName; // "woodCrateTex";
		//InTexture->Filename = InFilePath;// L"../../Textures/WoodCrate01.dds";

		//ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(GetParent()->GetDeviceInterface(), GetCommnadList()->GetGraphicsInterface(), InFilePath.c_str(), InResource->Get(), InResource->GetTextureResource()->Get()));
	}
}

void D3D12RHI::CreateDepthStencilView(D3D12Resource* InResource, class D3D12Descriptor* InDescriptor, D3D12_DEPTH_STENCIL_VIEW_DESC& InDepthStencilDesc)
{
	assert(InDescriptor);
	assert(InResource);

	InResource->GetDevice()->CreateDepthStencilView(InResource->GetResource(), &InDepthStencilDesc, InDescriptor->GetCpuHandle(0));
}