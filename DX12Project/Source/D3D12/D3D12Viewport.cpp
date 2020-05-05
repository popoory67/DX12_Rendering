#include "stdafx.h"
#include "D3D12Viewport.h"
#include "D3DUtil.h"
#include "d3dx12.h"
#include "D3D12Resource.h"
#include "D3D12Descriptor.h"
#include "D3D12Commands.h"
#include "D3D12RenderInterface.h"
#include "Application.h"

D3D12_VIEWPORT D3D12Viewport::ScreenViewport;

bool D3D12Viewport::IsMsaa4xState = false;
UINT D3D12Viewport::Msaa4xQuality = 0;

D3D12Viewport::D3D12Viewport(D3D12DeviceChild* InDevice)
	: D3D12DeviceChild(*InDevice)
{
	ScreenViewport.Width = 800;
	ScreenViewport.Height = 600;

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = GetSwapChainBufferCount();
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;

	SwapChainBufferDesc = new D3D12Descriptor(this, rtvHeapDesc);

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;

	DepthStencilBufferDesc = new D3D12Descriptor(this, dsvHeapDesc);

	CreateSwapChain();

	CreateDepthStencilBuffer();
	CreateSwapChainBuffer();
}

D3D12Resource* D3D12Viewport::GetCurrentBackBuffer() const
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
		SwapChainBufferDesc->GetCpuHandle(0),
		CurBackBufferIndex,
		(UINT)SwapChainBufferDesc->GetSize());
}

unsigned int D3D12Viewport::GetSwapChainBufferCount() const
{
	return SwapChainBufferCount; 
}

DXGI_FORMAT D3D12Viewport::GetDepthStencilFormat() const
{
	return DepthStencilFormat;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12Viewport::GetDepthStencilBufferView() const
{
	assert(DepthStencilBufferDesc);
	return DepthStencilBufferDesc->GetCpuHandle(0);
}

void D3D12Viewport::CreateSwapChainBuffer()
{
	if (SwapChainBufferDesc)
	{
		memset(SwapChainBuffer, 0x00, _countof(SwapChainBuffer));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(SwapChainBufferDesc->GetCpuHandle(0));

		for (int i = 0; i < _countof(SwapChainBuffer); ++i)
		{
			if (!SwapChainBuffer[i])
			{
				SwapChainBuffer[i] = new D3D12Resource();

				ThrowIfFailed(SwapChain->GetBuffer(i, IID_PPV_ARGS(&SwapChainBuffer[i]->Get())));

				GetParent()->CreateRenderTargetView(SwapChainBuffer[i], nullptr, rtvHeapHandle);
				rtvHeapHandle.Offset(1, (UINT)SwapChainBufferDesc->GetSize());
			}
		}
	}
}

void D3D12Viewport::CreateDepthStencilBuffer()
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;

	DepthStencilBufferDesc = new D3D12Descriptor(this, dsvHeapDesc);
	if (DepthStencilBufferDesc)
	{
		// Create the depth/stencil buffer and view.
		D3D12_RESOURCE_DESC desc;
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Alignment = 0;
		desc.Width = (UINT64)GetWidth();
		desc.Height = (UINT)GetHeight();
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;

		// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
		// the depth buffer.  Therefore, because we need to create two views to the same resource:
		//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
		//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
		// we need to create the depth buffer resource with a typeless format.  
		desc.Format = DXGI_FORMAT_R24G8_TYPELESS;

		desc.SampleDesc.Count = IsMsaa4xEnabled() ? 4 : 1;
		desc.SampleDesc.Quality = IsMsaa4xEnabled() ? (GetMsaaQuality() - 1) : 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = DepthStencilFormat;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;

		// Transition the resource from its initial state to be used as a depth buffer.
		DepthStencilBuffer = new D3D12Resource();

		GetParent()->CreateCommittedResource(
			DepthStencilBuffer,
			D3D12_HEAP_TYPE_DEFAULT,
			D3D12_HEAP_FLAG_NONE,
			desc,
			D3D12_RESOURCE_STATE_COMMON,
			optClear);

		GetParent()->GetCommandList()->ResourceBarrier(DepthStencilBuffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);

		// Create descriptor to mip level 0 of entire resource using the format of the resource.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DepthStencilFormat;
		dsvDesc.Texture2D.MipSlice = 0;

		GetParent()->CreateDepthStencilView(DepthStencilBuffer, DepthStencilBufferDesc, dsvDesc);
	}
}

void D3D12Viewport::OnResize()
{
	assert(SwapChain);
	assert(SwapChainBufferDesc);

	// Release the previous resources we will be recreating.
	for (int i = 0; i < _countof(SwapChainBuffer); ++i)
	{
		if (SwapChainBuffer[i])
			SwapChainBuffer[i]->Reset();
	}

	// Resize the swap chain.
	ThrowIfFailed(SwapChain->ResizeBuffers(
		_countof(SwapChainBuffer),
		(UINT)ScreenViewport.Width,
		(UINT)ScreenViewport.Height,
		BackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	CurBackBufferIndex = 0;

	CreateSwapChainBuffer();

	DepthStencilBuffer->Reset();
	CreateDepthStencilBuffer();
}

void D3D12Viewport::CreateSwapChain()
{
	// Release the previous swapchain we will be recreating.
	SwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC SwapChainBufferDesc;
	SwapChainBufferDesc.BufferDesc.Width = (UINT)ScreenViewport.Width;
	SwapChainBufferDesc.BufferDesc.Height = (UINT)ScreenViewport.Height;
	SwapChainBufferDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainBufferDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainBufferDesc.BufferDesc.Format = BackBufferFormat;
	SwapChainBufferDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainBufferDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainBufferDesc.SampleDesc.Count = IsMsaa4xState?4:1;
	SwapChainBufferDesc.SampleDesc.Quality = IsMsaa4xState?(Msaa4xQuality - 1):0;
	SwapChainBufferDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainBufferDesc.BufferCount = SwapChainBufferCount;
	SwapChainBufferDesc.OutputWindow = Application::GetWindowHandle(); // GetParent()->GetWindowHandle();
	SwapChainBufferDesc.Windowed = true;
	SwapChainBufferDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainBufferDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Note: Swap chain uses queue to perform flush.
	GetParent()->CreateSwapChain(SwapChain, SwapChainBufferDesc);

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render target formats, 
	// so we only need to check quality support.
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MultiSampleQualityLevels;
	MultiSampleQualityLevels.Format = BackBufferFormat;
	MultiSampleQualityLevels.SampleCount = 4;
	MultiSampleQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	MultiSampleQualityLevels.NumQualityLevels = 0;

	GetParent()->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, MultiSampleQualityLevels);

	Msaa4xQuality = MultiSampleQualityLevels.NumQualityLevels;

	assert(Msaa4xQuality > 0 && "Unexpected MSAA quality level."); // ??
}

void D3D12Viewport::SwapBackBufferToFrontBuffer()
{
	// Swap the back and front buffers
	ThrowIfFailed(SwapChain->Present(0, 0)); // ÀüÈ¯
	CurBackBufferIndex = (CurBackBufferIndex + 1) % SwapChainBufferCount;

	// Advance the fence value to mark commands up to this fence point.
//	CurFrameResource->Fence = ++CurrentFenceCount;
}

float D3D12Viewport::AspectRatio()const
{
	return static_cast<float>(ScreenViewport.Width) / ScreenViewport.Height;
}

void D3D12Viewport::UpdateViewport()
{
	GetParent()->GetCommandList()->Get()->RSSetViewports(1, &GetViewport());
	GetParent()->GetCommandList()->Get()->RSSetScissorRects(1, &GetRect());
}

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

void D3D12Viewport::ReadyToRenderTarget()
{
	D3D12Device* pDevice = GetParent();
	if (pDevice)
	{
		// Indicate a state transition on the resource usage.
		pDevice->GetCommandList()->ResourceBarrier(GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		// Clear the back buffer and depth buffer.
		pDevice->GetCommandList()->ClearRenderTargetView(GetCurrentBackBufferView(), Colors::LightSteelBlue, 0);
		pDevice->GetCommandList()->ClearDepthStencilView(GetDepthStencilBufferView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0);

		// Specify the buffers we are going to render to.
		pDevice->GetCommandList()->SetRenderTargets(1, GetCurrentBackBufferView(), true, GetDepthStencilBufferView());
	}
}

void D3D12Viewport::FinishToRenderTarget()
{
	// Indicate a state transition on the resource usage.
	GetParent()->GetCommandList()->ResourceBarrier(GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}