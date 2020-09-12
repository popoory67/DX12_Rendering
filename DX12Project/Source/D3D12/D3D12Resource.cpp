#include "stdafx.h"
#include <d3d12.h>
#include "D3D12Resource.h"
#include "D3D12Device.h"
#include "D3D12Descriptor.h"
#include "DDSTextureLoader.h"

D3D12Resource::D3D12Resource(D3D12DeviceChild* InDevice, ID3D12Resource* InResource)
	: D3D12DeviceChild(*InDevice)
	, Resource(InResource)
	, GpuVirtualAddress(0)
{
	GpuVirtualAddress = Resource->GetGPUVirtualAddress();
	Desc = Resource->GetDesc();
}

void D3D12Resource::Reset()
{
	Resource.Reset();

	GpuVirtualAddress = 0;
}

// -------------------------------------------------------------------------------------------------------------------- //

D3D12ResourceLocation::D3D12ResourceLocation(D3D12DeviceChild* InDevice)
	: D3D12DeviceChild(*InDevice)
{
	
}

void D3D12ResourceLocation::SetResource(D3D12Resource* InResource)
{
	GpuVirtualAddress = InResource->GetGPUVirtualAddress();
	Resource = InResource;
}

// -------------------------------------------------------------------------------------------------------------------- //

D3D12VertexBuffer::D3D12VertexBuffer(D3D12DeviceChild* InDevice)
	: D3D12DeviceChild(*InDevice)
{

}

// -------------------------------------------------------------------------------------------------------------------- //
//
//void D3D12DefaultResource::CreateDefaultBuffer(const void* InInitData, UINT64 InByteSize)
//{
//	D3D12Resource* pDefaultResource = new D3D12Resource();
//
//	// Create the actual default buffer resource.
//	GetParent()->CreateCommittedResource(pDefaultResource, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_NONE, InByteSize, D3D12_RESOURCE_STATE_COMMON);
//
//	// In order to copy CPU memory data into our default buffer, we need to create an intermediate upload heap. 
//	GetParent()->CreateCommittedResource(UploadResource, D3D12_HEAP_TYPE_UPLOAD, D3D12_HEAP_FLAG_NONE, InByteSize, D3D12_RESOURCE_STATE_GENERIC_READ);
//
//	// Describe the data we want to copy into the default buffer.
//	D3D12_SUBRESOURCE_DATA subResourceData = {};
//	subResourceData.pData = InInitData;
//	subResourceData.RowPitch = InByteSize;
//	subResourceData.SlicePitch = subResourceData.RowPitch;
//
//	// Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
//	// will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
//	// the intermediate upload heap data will be copied to mBuffer.
//	GetCommandList()->ResourceBarrier(pDefaultResource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
//
//	GetCommandList()->UpdateResources<1>(pDefaultResource, UploadResource, 0, 0, 1, subResourceData);
//
//	GetCommandList()->ResourceBarrier(pDefaultResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
//
//	// Note: UploadBuffer has to be kept alive after the above function calls because
//	// the command list has not been executed yet that performs the actual copy.
//	// The caller can Release the UploadBuffer after it knows the copy has been executed.
//}

// -------------------------------------------------------------------------------------------------------------------- //

//D3D12ShaderResource::D3D12ShaderResource(std::string InName/* = nullptr*/, std::wstring InFilePath/* = nullptr*/)
//{
//	// Create the SRV heap.
//	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
//	srvHeapDesc.NumDescriptors = 1;
//	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//
//	ShaderResourceDesc = new D3D12Descriptor(this, srvHeapDesc);
//
//	// ���ҽ� �׽�Ʈ
//	// �ؽ��ĸ� ��� �������� ����ؾ���
//	LoadTextures(InName, InFilePath);
//	assert(Resource);
//
//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//	srvDesc.Format = Resource->GetDesc().Format;
//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
//	srvDesc.Texture2D.MostDetailedMip = 0;
//	srvDesc.Texture2D.MipLevels = Resource->GetDesc().MipLevels;
//	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
//
//	GetParent()->CreateShaderView(Resource, ShaderResourceDesc, srvDesc);
//}

// 
// void D3DApp::BuildMaterials()
// {
// 	auto woodCrate = std::make_unique<Material>();
// 	woodCrate->Name = "woodCrate";
// 	woodCrate->MatCBIndex = 0;
// 	woodCrate->DiffuseSrvHeapIndex = 0;
// 	woodCrate->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
// 	woodCrate->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
// 	woodCrate->Roughness = 0.2f;
// 
// 	mMaterials["woodCrate"] = std::move(woodCrate);
// }
// 

//void D3D12ShaderResource::LoadTextures(std::string InName/* = nullptr*/, std::wstring InFilePath/* = nullptr*/)
//{
//	Texture = std::make_unique<TextureData>();
//	if (Texture)
//	{
//		Texture->Name = InName; // "woodCrateTex";
//		Texture->Filename = InFilePath;// L"../../Textures/WoodCrate01.dds";
//
//		ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(GetParent()->GetDeviceInterface(), GetCommandList()->GetGraphicsInterface(), Texture->Filename.c_str(), Resource, UploadTextureResource->Get()));
//	}
//}

UINT64 D3D12ShaderResource::GetDescriptorHandleIncrementSize()
{
	assert(ShaderResourceDesc);
	return ShaderResourceDesc->GetSize();
}

// -------------------------------------------------------------------------------------------------------------------- //