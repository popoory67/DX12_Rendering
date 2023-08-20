
#include <DirectXColors.h>
#include "d3dx12.h"
#include "D3D12Viewport.h"
#include "D3D12Resource.h"
#include "D3D12Descriptor.h"
#include "D3D12Commands.h"
#include "D3D12RenderInterface.h"
#include "D3D12Fence.h"

#include "D3DUtil.h"

bool D3D12Viewport::IsMsaa4xState = false;
UINT D3D12Viewport::Msaa4xQuality = 0;

D3D12Viewport::D3D12Viewport(D3D12Device* InDevice, HWND InHandle, unsigned int InWidth, unsigned int InHeight)
	: D3D12Api(InDevice)
	, WindowHandle(InHandle)
	, BackBufferFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
	//, DepthStencilFormat(DXGI_FORMAT_D24_UNORM_S8_UINT)
{
	ScreenViewport.Width = static_cast<float>(InWidth);
	ScreenViewport.Height = static_cast<float>(InHeight);

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = GetSwapChainBufferCount();
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;

	SwapChainBufferDescriptor = std::make_shared<D3D12Descriptor>(GetParent(), rtvHeapDesc);

	//D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	//dsvHeapDesc.NumDescriptors = 1;
	//dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	//dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	//dsvHeapDesc.NodeMask = 0;

	//DepthStencilBufferDescriptor = std::make_shared<D3D12Descriptor>(GetParent(), dsvHeapDesc);

	CreateSwapChain();

	// TODO
	// It has to be modified to pooling codes.
	for (int i = 0; i < SwapChainBufferCount; ++i)
	{
		Fence[i] = new D3D12Fence(InDevice);
		Fence[i]->Signal();
	}
}

D3D12Viewport::~D3D12Viewport()
{
	WindowHandle = nullptr;

    for (int i = 0; i < SwapChainBufferCount; ++i)
    {
        SafeDelete(Fence[i]);
    }
}

std::shared_ptr<D3D12Resource> D3D12Viewport::GetCurrentBackBuffer() const
{
	if (SwapChainBuffer[CurBackBufferIndex])
	{
		return SwapChainBuffer[CurBackBufferIndex];
	}
	return nullptr;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12Viewport::GetCurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		SwapChainBufferDescriptor->GetCpuHandle(0),
		CurBackBufferIndex,
		(UINT)SwapChainBufferDescriptor->GetSize());
}

constexpr unsigned int D3D12Viewport::GetSwapChainBufferCount()
{
	return SwapChainBufferCount; 
}

constexpr float D3D12Viewport::GetAspectRatio()
{
	return static_cast<float>(ScreenViewport.Width) / ScreenViewport.Height;
}

void D3D12Viewport::Present()
{
	const auto& backBuffer = GetCurrentBackBuffer();
	if (backBuffer)
	{
		if (D3D12Device* device = GetParent())
		{			
			// TODO
			// I only use one command list now, but it must be changed to several lists.
			D3D12CommandList& commandList = device->GetCommandList();

			commandList.AddTransition(backBuffer, D3D12_RESOURCE_STATE_PRESENT);

			// Before a command queue executes command lists,
			// that lists have to be closed. (EXECUTECOMMANDLISTS_OPENCOMMANDLIST)
			commandList.Close();
			device->GetCommandListExecutor().ExecuteCommandLists(&commandList);
		}
	}

	// Swap the back and front buffers
	ThrowIfFailed(SwapChain->Present(1, 0));

	// TODO
	// The moment the GPU Signal is called must be separately invoked when the CPU thread stalls with the WaitForSingleObject.
	// An available scenario is that the GPU Signal have to be called in the render thread, while the main (game) thread is managing the CPU stall.
	EndFrame();
	WaitForFrameCompletion();

	CurBackBufferIndex = (CurBackBufferIndex + 1) % SwapChainBufferCount;
}

//DXGI_FORMAT D3D12Viewport::GetDepthStencilFormat() const
//{
//	return DepthStencilFormat;
//}
//
//D3D12_CPU_DESCRIPTOR_HANDLE D3D12Viewport::GetDepthStencilBufferView() const
//{
//	assert(DepthStencilBufferDescriptor);
//	return DepthStencilBufferDescriptor->GetCpuHandle(0);
//}

void D3D12Viewport::Resize()
{
	// Release the previous resources we will be recreating.
	for (int i = 0; i < SwapChainBufferCount; ++i)
	{
		if (SwapChainBuffer[i])
		{
			SwapChainBuffer[i]->Reset();
		}
	}

	// Resize the swap chain.
	ThrowIfFailed(SwapChain->ResizeBuffers(
		SwapChainBufferCount,
		(UINT)ScreenViewport.Width,
		(UINT)ScreenViewport.Height,
		BackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	CurBackBufferIndex = SwapChain->GetCurrentBackBufferIndex();

	CreateSwapChainBuffer();

	//DepthStencilBuffer->Reset();

	//CreateDepthStencilBuffer();
}

void D3D12Viewport::CreateSwapChain()
{
	// Release the previous swapchain we will be recreating.
	SwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc.Width = (UINT)ScreenViewport.Width;
	swapChainDesc.BufferDesc.Height = (UINT)ScreenViewport.Height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = BackBufferFormat;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// MSAA Sample count
	swapChainDesc.SampleDesc.Count = IsMsaa4xState ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = IsMsaa4xState ? (Msaa4xQuality - 1) : 0;

	swapChainDesc.BufferCount = SwapChainBufferCount;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = WindowHandle;
	swapChainDesc.Windowed = true;

	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Note: Swap chain uses queue to perform flush.
	if (auto pDevice = GetParent())
	{
		auto commandQueue = pDevice->GetCommandQueue();

		ComPtr<IDXGISwapChain> swapChain;
		ThrowIfFailed(GetParent()->GetDxgi()->CreateSwapChain(commandQueue, &swapChainDesc, &swapChain));

		ThrowIfFailed(swapChain->QueryInterface(IID_PPV_ARGS(SwapChain.GetAddressOf())));

		// Check 4X MSAA quality support for our back buffer format.
		// All Direct3D 11 capable devices support 4X MSAA for all render target formats, 
		// so we only need to check quality support.
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MultiSampleQualityLevels;
		MultiSampleQualityLevels.Format = BackBufferFormat;
		MultiSampleQualityLevels.SampleCount = 4;
		MultiSampleQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		MultiSampleQualityLevels.NumQualityLevels = 0;

		ThrowIfFailed(GetDevice()->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &MultiSampleQualityLevels, sizeof(MultiSampleQualityLevels)));

		Msaa4xQuality = MultiSampleQualityLevels.NumQualityLevels;

		assert(Msaa4xQuality > 0 && "Unexpected MSAA quality level."); // ??
	}

	::PostMessage(WindowHandle, WM_PAINT, 0, 0);
}

void D3D12Viewport::CreateSwapChainBuffer()
{
	if (SwapChainBufferDescriptor)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(SwapChainBufferDescriptor->GetCpuHandle(0));

		for (int i = 0; i < SwapChainBufferCount; ++i)
		{
			ComPtr<ID3D12Resource> backBufferResource;
			ThrowIfFailed(SwapChain->GetBuffer(i, IID_PPV_ARGS(backBufferResource.GetAddressOf())));

			SwapChainBuffer[i] = std::make_shared<D3D12Resource>(backBufferResource.Get());

			SwapChainBuffer[i]->SetResourceState(D3D12_RESOURCE_STATE_PRESENT);

			GetDevice()->CreateRenderTargetView(SwapChainBuffer[i]->GetResource(), nullptr, rtvHeapHandle);
			rtvHeapHandle.Offset(1, (UINT)SwapChainBufferDescriptor->GetSize());

		}
	}
}

void D3D12Viewport::WaitForFrameCompletion()
{
	Fence[CurBackBufferIndex]->CpuWait(CurrentFence);
}

void D3D12Viewport::EndFrame()
{
	CurrentFence = Fence[CurBackBufferIndex]->Signal();
}

//void D3D12Viewport::CreateDepthStencilBuffer()
//{
//	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
//	dsvHeapDesc.NumDescriptors = 1;
//	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
//	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//	dsvHeapDesc.NodeMask = 0;
//
//	if (DepthStencilBufferDescriptor)
//	{
//		// Create the depth/stencil buffer and view.
//		D3D12_RESOURCE_DESC desc;
//		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
//		desc.Alignment = 0;
//		desc.Width = (UINT64)GetWidth();
//		desc.Height = (UINT)GetHeight();
//		desc.DepthOrArraySize = 1;
//		desc.MipLevels = 1;
//
//		// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
//		// the depth buffer.  Therefore, because we need to create two views to the same resource:
//		//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
//		//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
//		// we need to create the depth buffer resource with a typeless format.  
//		desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
//
//		desc.SampleDesc.Count = IsMsaa4xEnabled() ? 4 : 1;
//		desc.SampleDesc.Quality = IsMsaa4xEnabled() ? (GetMsaaQuality() - 1) : 0;
//		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
//		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
//
//		D3D12_CLEAR_VALUE optClear;
//		optClear.Format = DepthStencilFormat;
//		optClear.DepthStencil.Depth = 1.0f;
//		optClear.DepthStencil.Stencil = 0;
//
//		// Transition the resource from its initial state to be used as a depth buffer.
//		ComPtr<ID3D12Resource> depthStencilBuffer;
//		D3D12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
//
//		ThrowIfFailed(GetDevice()->CreateCommittedResource(
//			&properties,
//			D3D12_HEAP_FLAG_NONE,
//			&desc,
//			D3D12_RESOURCE_STATE_COMMON,
//			&optClear,
//			IID_PPV_ARGS(&depthStencilBuffer)));
//
//		DepthStencilBuffer = std::make_shared<D3D12Resource>(GetParent(), depthStencilBuffer.Get());
//
//		GetParent()->GetCommandList().AddTransition(DepthStencilBuffer->GetResource(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
//
//		// Create descriptor to mip level 0 of entire resource using the format of the resource.
//		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
//		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
//		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
//		dsvDesc.Format = DepthStencilFormat;
//		dsvDesc.Texture2D.MipSlice = 0;
//
//		GetDevice()->CreateDepthStencilView(DepthStencilBuffer->GetResource(), &dsvDesc, DepthStencilBufferDescriptor->GetCpuHandle(0));
//	}
//}

void D3D12Viewport::SetViewport(D3DViewportResource& InViewResource)
{
	D3D12_VIEWPORT& viewport = GetViewport();

	viewport.TopLeftX = InViewResource.TopLeftX;
	viewport.TopLeftY = InViewResource.TopLeftY;
	viewport.Width = InViewResource.Width;
	viewport.Height = InViewResource.Height;
	viewport.MinDepth = InViewResource.MinDepth;
	viewport.MaxDepth = InViewResource.MaxDepth;

	D3D12_RECT& ScissorRect = GetRect();

	ScissorRect = { 0, 0, (LONG)viewport.Width, (LONG)viewport.Height };
}

RHIViewport* D3D12RHI::CreateViewport(void* InHWnd, unsigned int InWidth, unsigned int InHeight)
{
	D3D12Viewport* viewport = new D3D12Viewport(GetCurrentDevice(), (HWND)InHWnd, InWidth, InHeight);

	// Update the viewport transform to cover the client area.
	D3DViewportResource screenViewport;
	screenViewport.TopLeftX = 0;
	screenViewport.TopLeftY = 0;
	screenViewport.Width = (float)InWidth;
	screenViewport.Height = (float)InHeight;
	screenViewport.MinDepth = 0.0f;
	screenViewport.MaxDepth = 1.0f;

	viewport->SetViewport(screenViewport);

	viewport->Resize();

	return viewport;
}

void D3D12RHI::ResizeViewport(RHIViewport* InViewportRHI)
{
	D3D12Viewport* viewport = static_cast<D3D12Viewport*>(InViewportRHI);
	assert(viewport);

	viewport->Resize();
}
