#include "stdafx.h"
#include "D3D12Device.h"
#include "D3D12Commands.h"
#include "D3D12SwapChain.h"

D3D12Device::D3D12Device(HWND InWindowHandle)
{
	MainWindowHandle = InWindowHandle;
	assert(MainWindowHandle);

	CreateDevice();
}

D3D12Device::~D3D12Device()
{
}

void D3D12Device::CreateDevice()
{
#if defined(DEBUG) || defined(_DEBUG) 
	// Enable the D3D12 debug layer.
	{
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif

	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&DxgiFactory)));

	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,             // default adapter
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&Device));

	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(DxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&Device)));
	}
}

void D3D12Device::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE InCommandListType, ComPtr<ID3D12CommandAllocator>& InCommandAllocator)
{
	ThrowIfFailed(Get()->CreateCommandAllocator(InCommandListType, IID_PPV_ARGS(InCommandAllocator.GetAddressOf())));
}

void D3D12Device::CreateCommandList(D3D12_COMMAND_LIST_TYPE InCommandListType, ComPtr<ID3D12CommandAllocator>& InCommandAllocator, ComPtr<ID3D12GraphicsCommandList>& InCommandList)
{
	CreateCommandAllocator(InCommandListType, InCommandAllocator);
	
	ThrowIfFailed(Get()->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		InCommandAllocator.Get(), // Associated command allocator
		nullptr,                   // Initial PipelineStateObject
		IID_PPV_ARGS(InCommandList.GetAddressOf())));
}

void D3D12Device::CreateCommandQueue(D3D12_COMMAND_QUEUE_DESC& InQueueDesc, ComPtr<ID3D12CommandQueue>& InCommandQueue)
{
	ThrowIfFailed(Get()->CreateCommandQueue(&InQueueDesc, IID_PPV_ARGS(&InCommandQueue)));
}

void D3D12Device::CreateSwapChain(D3D12CommandListExecutor* InExecutor, D3D12SwapChain* InSwapChain, DXGI_SWAP_CHAIN_DESC& InSwapChainDesc)
{
	assert(InExecutor);
	assert(InSwapChain);

	ThrowIfFailed(GetDxgi()->CreateSwapChain(InExecutor->GetExecutorInterface(), &InSwapChainDesc, InSwapChain->Get().GetAddressOf()));
}

void D3D12Device::CheckFeatureSupport(D3D12_FEATURE InFeature, D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS& InMultisampleQualityLevels)
{
	ThrowIfFailed(Get()->CheckFeatureSupport(InFeature, &InMultisampleQualityLevels, sizeof(InMultisampleQualityLevels)));
}

void D3D12Device::CreateCommittedResource(ComPtr<ID3D12Resource>& InResource, D3D12_HEAP_TYPE InHeapType, D3D12_HEAP_FLAGS InHeapFlags, D3D12_RESOURCE_DESC& InDesc, D3D12_RESOURCE_STATES InResourceStates, D3D12_CLEAR_VALUE InValue)
{
	ThrowIfFailed(Get()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(InHeapType),
		InHeapFlags,
		&InDesc,
		InResourceStates,
		&InValue,
		IID_PPV_ARGS(InResource.GetAddressOf())));
}

void D3D12Device::CreateRenderTargetView(ComPtr<ID3D12Resource>& InResource, const D3D12_RENDER_TARGET_VIEW_DESC* InDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE& InDescriptorHandle)
{
	Get()->CreateRenderTargetView(InResource.Get(), InDesc, InDescriptorHandle);
}

void D3D12Device::CreateDepthStencilView(ComPtr<ID3D12Resource>& InResource, class D3D12Descriptor* InDescriptor, D3D12_DEPTH_STENCIL_VIEW_DESC& InDepthStencilDesc)
{
	assert(InDescriptor);

	Get()->CreateDepthStencilView(InResource.Get(), &InDepthStencilDesc, InDescriptor->GetDescriptorHandle());
}

void D3D12Device::CreateShaderView(ComPtr<ID3D12Resource>& InResource, class D3D12Descriptor* InDescriptor, D3D12_SHADER_RESOURCE_VIEW_DESC& InShaderDesc)
{
	assert(InDescriptor);

	Get()->CreateShaderResourceView(InResource.Get(), &InShaderDesc, InDescriptor->GetDescriptorHandle());
}

UINT D3D12Device::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE InDescriptorHeapType)
{
	return Get()->GetDescriptorHandleIncrementSize(InDescriptorHeapType);
}