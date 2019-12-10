#include "stdafx.h"
#include <intsafe.h>
#include <d3d12.h>
#include "d3dx12.h"
#include "D3D12Resource.h"
#include "D3D12Device.h"
#include "D3D12Descriptor.h"
#include "D3D12SwapChain.h"
#include "D3D12Commands.h"
#include "DDSTextureLoader.h"

D3D12Resource::D3D12Resource()
{

}

D3D12Resource::D3D12Resource(D3D12Device* InDevice, D3D12_RESOURCE_DESC InDesc, D3D12_CLEAR_VALUE InValue)
{
	CreateResource(InDevice, InDesc, InValue);
}

D3D12Resource::D3D12Resource(D3D12Device* InDevice, UINT64 InByteSize, D3D12_CLEAR_VALUE InValue)
{
	CreateResource(InDevice, InByteSize, InValue);
}

void D3D12Resource::Reset()
{
	Resource.Reset();
}

void D3D12Resource::CreateResource(D3D12Device* InDevice, D3D12_RESOURCE_DESC InDesc, D3D12_CLEAR_VALUE InValue)
{
	assert(InDevice);

	Resource.Reset();

	InDevice->CreateCommittedResource(
		Resource,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_HEAP_FLAG_NONE,
		InDesc,
		D3D12_RESOURCE_STATE_COMMON,
		InValue);
}

void D3D12Resource::CreateResource(D3D12Device* InDevice, UINT64 InByteSize, D3D12_CLEAR_VALUE InValue)
{
	assert(InDevice);

	Resource.Reset();

	CD3DX12_RESOURCE_DESC desc;
	memcpy((void*)&desc, (void*)&CD3DX12_RESOURCE_DESC::Buffer(InByteSize), sizeof(CD3DX12_RESOURCE_DESC));

	InDevice->CreateCommittedResource(
		Resource,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_HEAP_FLAG_NONE,
		desc,
		D3D12_RESOURCE_STATE_COMMON,
		InValue);
}

// D3D12RenderTargetResource::D3D12RenderTargetResource(class D3D12Device* InDevice, CD3DX12_CPU_DESCRIPTOR_HANDLE& InDescriptorHandle, UINT InDescriptorSize)
// {
// 	assert(InDevice);
// 
// 	InDevice->CreateRenderTargetView(Resource, nullptr, InDescriptorHandle);
// 	InDescriptorHandle.Offset(1, InDescriptorSize);
// }

D3D12RenderTargetResource::D3D12RenderTargetResource(D3D12Device* InDevice, D3D12SwapChain* InSwapChain, CD3DX12_CPU_DESCRIPTOR_HANDLE& InDescriptorHandle, UINT InDescriptorSize, unsigned int InIndex)
{
	assert(InDevice || InSwapChain);

	if (InSwapChain->Get())
		ThrowIfFailed(InSwapChain->Get()->GetBuffer(InIndex, IID_PPV_ARGS(&Resource)));

	InDevice->CreateRenderTargetView(Resource, nullptr, InDescriptorHandle);
	InDescriptorHandle.Offset(1, InDescriptorSize);
}

D3D12DepthStencilResource::D3D12DepthStencilResource(D3D12Device* InDevice, D3D12CommandList* InCommandList)
{
	assert(InDevice);
	assert(InCommandList);

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;

	DepthStencilDesc = new D3D12Descriptor(InDevice, dsvHeapDesc, D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	if (DepthStencilDesc)
	{
		// Create the depth/stencil buffer and view.
		D3D12_RESOURCE_DESC depthStencilDesc;
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = (UINT64)D3D12SwapChain::GetWidth();
		depthStencilDesc.Height = (UINT)D3D12SwapChain::GetHeight();
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;

		// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
		// the depth buffer.  Therefore, because we need to create two views to the same resource:
		//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
		//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
		// we need to create the depth buffer resource with a typeless format.  
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

		depthStencilDesc.SampleDesc.Count = D3D12SwapChain::IsMsaa4xEnabled() ? 4 : 1;
		depthStencilDesc.SampleDesc.Quality = D3D12SwapChain::IsMsaa4xEnabled() ? (D3D12SwapChain::GetMsaaQuality() - 1) : 0;
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = DepthStencilFormat;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;

		// Transition the resource from its initial state to be used as a depth buffer.
		CreateResource(InDevice, depthStencilDesc, optClear);

		InCommandList->ResourceBarrier(this, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);

		// Create descriptor to mip level 0 of entire resource using the format of the resource.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DepthStencilFormat;
		dsvDesc.Texture2D.MipSlice = 0;

		InDevice->CreateDepthStencilView(Resource, DepthStencilDesc, dsvDesc);
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12DepthStencilResource::GetDepthStencilView() const
{
	assert(DepthStencilDesc);

	return DepthStencilDesc->GetDescriptorHandle();
}

D3D12ShaderResource::D3D12ShaderResource(D3D12Device* InDevice, D3D12CommandList* InCommandList/* = nullptr*/, std::string InName/* = nullptr*/, std::wstring InFilePath/* = nullptr*/)
{
	assert(InDevice);

	// Create the SRV heap.
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	ShaderResourceDesc = new D3D12Descriptor(InDevice, srvHeapDesc, D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 리소스 테스트
	// 텍스쳐를 어떻게 정리할지 고민해야함
	LoadTextures(InDevice, InCommandList, InName, InFilePath);
	assert(Resource);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = Resource->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = Resource->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	InDevice->CreateShaderView(Resource, ShaderResourceDesc, srvDesc);
}

void D3D12ShaderResource::LoadTextures(class D3D12Device* InDevice, class D3D12CommandList* InCommandList, std::string InName/* = nullptr*/, std::wstring InFilePath/* = nullptr*/)
{
	assert(InDevice);

	if (InCommandList)
	{
		Texture = std::make_unique<TextureData>();
		if (Texture)
		{
			Texture->Name = InName; // "woodCrateTex";
			Texture->Filename = InFilePath;// L"../../Textures/WoodCrate01.dds";

			ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(InDevice->GetInterface(), InCommandList->GetGraphics(), Texture->Filename.c_str(), Resource, Texture->UploadHeap));
		}
	}
}

UINT D3D12ShaderResource::GetDescriptorHandleIncrementSize()
{
	assert(ShaderResourceDesc);
	return ShaderResourceDesc->GetSize();
}
