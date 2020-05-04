#include "stdafx.h"
#include "D3D12RenderInterface.h"
#include "D3D12Device.h"
#include "D3D12Commands.h"
#include "D3D12Descriptor.h"
#include "D3D12Viewport.h"
#include "D3D12Resource.h"
#include "D3D12PipelineState.h"
#include "DDSTextureLoader.h"
#include "Texture.h"

using namespace DirectX;

D3D12RenderInterface::D3D12RenderInterface(D3D12DeviceChild* InDevice)
	: D3D12DeviceChild(*InDevice)
{
	assert(InDevice);

	// test
	//D3D12PipelineState* pPipelineState = new D3D12PipelineState(InDevice, GetBackBufferFormat(), GetDepthStencilFormat());
	//if (pPipelineState)
	//{
	//	PipelineStates.emplace_back(pPipelineState);
	//}
}

D3D12RenderInterface::~D3D12RenderInterface()
{

}

//void D3D12RenderInterface::OnResize()
//{
//	assert(CommandList);
//
//	CommandList->Reset();
//
////	SwapChain->OnResize();
//
//	ExecuteCommandList();
//	FlushCommandQueue();
//}

void D3D12RenderInterface::ResetCommandList()
{
	GetCommnadList()->Reset();
}

void D3D12RenderInterface::ExecuteCommandList()
{
	D3D12Device* pDevice = GetParent();
	if (pDevice)
	{
		for (D3D12PipelineState* pPso : PipelineStates)
		{
			pDevice->GetCommandList()->SetPipelineState(pPso);
		}

		pDevice->ExecuteCommands();
	}
}

void D3D12RenderInterface::FlushCommandQueue() const
{
	GetParent()->FlushCommandQueue();
}

//void D3D12RenderInterface::UpdateViewport(D3D12Viewport* InSwapChain)
//{
//	assert(CommandList);
//	assert(InSwapChain);
//
//	CommandList->Get()->RSSetViewports(1, &InSwapChain->GetViewport());
//	CommandList->Get()->RSSetScissorRects(1, &InSwapChain->GetRect());
//}
//
//void D3D12RenderInterface::SetViewport(D3DViewportResource& InViewResource, D3D12Viewport* InSwapChain)
//{
//	assert(InSwapChain);
//
//	D3D12_VIEWPORT& viewport = InSwapChain->GetViewport();
//
//	viewport.TopLeftX = InViewResource.TopLeftX;
//	viewport.TopLeftY = InViewResource.TopLeftY;
//	viewport.Width = InViewResource.Width;
//	viewport.Height = InViewResource.Height;
//	viewport.MinDepth = InViewResource.MinDepth;
//	viewport.MaxDepth = InViewResource.MaxDepth;
//
//	D3D12_RECT& ScissorRect = InSwapChain->GetRect();
//
//	ScissorRect = { 0, 0, (LONG)viewport.Width, (LONG)viewport.Height };
//}

//void D3D12RenderInterface::ReadyToRenderTarget(D3D12Resource* InBackBuffer, D3D12_CPU_DESCRIPTOR_HANDLE InBackBufferView, D3D12_CPU_DESCRIPTOR_HANDLE InDepthStencilBuffer)
//{
//	assert(CommandList);
//
//	// Indicate a state transition on the resource usage.
//	CommandList->ResourceBarrier(InBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
//
//	// Clear the back buffer and depth buffer.
//	CommandList->ClearRenderTargetView(InBackBufferView, Colors::LightSteelBlue, 0);
//	CommandList->ClearDepthStencilView(InDepthStencilBuffer, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0);
//
//	// Specify the buffers we are going to render to.
//	CommandList->SetRenderTargets(1, InBackBufferView, true, InDepthStencilBuffer);
//}
//
//void D3D12RenderInterface::FinishToRenderTarget(D3D12Resource* InBackBuffer)
//{
//	assert(CommandList);
//	assert(InBackBuffer);
//
//	// Indicate a state transition on the resource usage.
//	CommandList->ResourceBarrier(InBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
//}

// -------------------------------------------------------------------------------------------------------------------- //

void D3D12RenderInterface::CreateResource(D3D12Resource* InResource, std::optional<D3D12_RESOURCE_DESC> InDesc /*= {}*/, std::optional<D3D12_CLEAR_VALUE> InValue /*= {}*/)
{
	assert(InResource);

	InResource->Reset();

	GetParent()->CreateCommittedResource(
		InResource,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_HEAP_FLAG_NONE,
		InDesc.value(),
		D3D12_RESOURCE_STATE_COMMON,
		InValue.value());
}

void D3D12RenderInterface::CreateResource(D3D12Resource* InResource, UINT64 InByteSize, std::optional<D3D12_CLEAR_VALUE> InValue/* = {}*/)
{
	assert(InResource);

	InResource->Reset();

	CD3DX12_RESOURCE_DESC desc;
	memcpy((void*)&desc, (void*)&CD3DX12_RESOURCE_DESC::Buffer(InByteSize), sizeof(CD3DX12_RESOURCE_DESC));

	GetParent()->CreateCommittedResource(
		InResource,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_HEAP_FLAG_NONE,
		desc,
		D3D12_RESOURCE_STATE_COMMON,
		InValue.value());
}

void D3D12RenderInterface::CreateDefaultBuffer(D3D12DefaultResource* InResource, const void* InInitData, UINT64 InByteSize)
{
	assert(InResource);

	D3D12Device* pDevice = GetParent();
	if (pDevice)
	{
		D3D12Resource* pDefaultResource = new D3D12Resource();

		// Create the actual default buffer resource.
		pDevice->CreateCommittedResource(pDefaultResource, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_NONE, InByteSize, D3D12_RESOURCE_STATE_COMMON);

		// In order to copy CPU memory data into our default buffer, we need to create an intermediate upload heap. 
		pDevice->CreateCommittedResource(InResource, D3D12_HEAP_TYPE_UPLOAD, D3D12_HEAP_FLAG_NONE, InByteSize, D3D12_RESOURCE_STATE_GENERIC_READ);

		// Describe the data we want to copy into the default buffer.
		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = InInitData;
		subResourceData.RowPitch = InByteSize;
		subResourceData.SlicePitch = subResourceData.RowPitch;

		// Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
		// will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
		// the intermediate upload heap data will be copied to mBuffer.
		pDevice->GetCommandList()->ResourceBarrier(pDefaultResource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
		pDevice->GetCommandList()->UpdateResources<1>(pDefaultResource, InResource, 0, 0, 1, subResourceData);
		pDevice->GetCommandList()->ResourceBarrier(pDefaultResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);

		// Note: UploadBuffer has to be kept alive after the above function calls because
		// the command list has not been executed yet that performs the actual copy.
		// The caller can Release the UploadBuffer after it knows the copy has been executed.
	}
}

void D3D12RenderInterface::CreateRenderTarget(D3D12RenderTargetResource* InResource, CD3DX12_CPU_DESCRIPTOR_HANDLE& InDescriptorHandle, UINT InDescriptorSize)
{
	assert(InResource);

	GetParent()->CreateRenderTargetView(InResource, nullptr, InDescriptorHandle);
	InDescriptorHandle.Offset(1, InDescriptorSize);
}

void D3D12RenderInterface::CreateDepthStencil(D3D12DepthStencilResource* InResource, D3D12Descriptor* InDescriptor)
{
	assert(InDescriptor);

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;

	InDescriptor = new D3D12Descriptor(this, dsvHeapDesc);
	if (InDescriptor)
	{
		// Create the depth/stencil buffer and view.
		D3D12_RESOURCE_DESC depthStencilDesc;
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = (UINT64)D3D12Viewport::GetWidth();
		depthStencilDesc.Height = (UINT)D3D12Viewport::GetHeight();
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;

		// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
		// the depth buffer.  Therefore, because we need to create two views to the same resource:
		//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
		//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
		// we need to create the depth buffer resource with a typeless format.  
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

		depthStencilDesc.SampleDesc.Count = D3D12Viewport::IsMsaa4xEnabled()?4:1;
		depthStencilDesc.SampleDesc.Quality = D3D12Viewport::IsMsaa4xEnabled()?(D3D12Viewport::GetMsaaQuality() - 1):0;
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = InResource->GetDepthStencilFormat();
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;

		// Transition the resource from its initial state to be used as a depth buffer.
		CreateResource(InResource, depthStencilDesc, optClear);

		GetCommnadList()->ResourceBarrier(InResource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);

		// Create descriptor to mip level 0 of entire resource using the format of the resource.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = InResource->GetDepthStencilFormat();
		dsvDesc.Texture2D.MipSlice = 0;

		GetParent()->CreateDepthStencilView(InResource, InDescriptor, dsvDesc);
	}
}

void D3D12RenderInterface::CreateShaderResource(D3D12ShaderResource* InResource, D3D12Descriptor* InDescriptor, std::string InName/* = nullptr*/, std::wstring InFilePath/* = nullptr*/)
{
	assert(InDescriptor);

	// Create the SRV heap.
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	InDescriptor = new D3D12Descriptor(this, srvHeapDesc);

	// 리소스 테스트
	// 텍스쳐를 어떻게 정리할지 고민해야함
	LoadTexture(InResource, InName, InFilePath);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = InResource->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = InResource->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	GetParent()->CreateShaderView(InResource, InResource->GetDescriptor(), srvDesc);
}

void D3D12RenderInterface::LoadTexture(D3D12ShaderResource* InResource, std::string InName, std::wstring InFilePath)
{
	if (InResource)
	{
		//InTexture->Name = InName; // "woodCrateTex";
		//InTexture->Filename = InFilePath;// L"../../Textures/WoodCrate01.dds";

		//ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(GetParent()->GetDeviceInterface(), GetCommnadList()->GetGraphicsInterface(), InFilePath.c_str(), InResource->Get(), InResource->GetTextureResource()->Get()));
	}
}
