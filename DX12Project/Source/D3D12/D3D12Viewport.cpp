#include "stdafx.h"
#include "D3D12Viewport.h"
#include "D3DUtil.h"
#include "d3dx12.h"
#include "D3D12Resource.h"
#include "D3D12Descriptor.h"
#include "D3D12Commands.h"
#include "D3D12RenderInterface.h"

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

	RenderTargetViewDesc = new D3D12Descriptor(this, rtvHeapDesc);

	DepthStencilBuffer = new D3D12DepthStencilResource();

	// on resize
	if (DepthStencilBuffer)
		DepthStencilBuffer->Reset();
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
		RenderTargetViewDesc->GetHeap()->GetCPUDescriptorHandleForHeapStart(),
		CurBackBufferIndex,
		(UINT)RenderTargetViewDesc->GetSize());
}

unsigned int D3D12Viewport::GetSwapChainBufferCount() const
{
	return SwapChainBufferCount; 
}

DXGI_FORMAT D3D12Viewport::GetDepthStencilFormat() const
{
	assert(DepthStencilBuffer);
	return DepthStencilBuffer->GetDepthStencilFormat();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12Viewport::GetDepthStencilBufferView() const
{
	assert(DepthStencilBuffer);
	return DepthStencilBuffer->GetDepthStencilView();
}

void D3D12Viewport::CreateBuffer()
{
	if (RenderTargetViewDesc)
	{
		memset(SwapChainBuffer, 0x00, _countof(SwapChainBuffer));

		ComPtr<ID3D12DescriptorHeap> pRenderTargetDesc = RenderTargetViewDesc->GetHeap();
		if (pRenderTargetDesc)
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(pRenderTargetDesc->GetCPUDescriptorHandleForHeapStart());

			for (int i = 0; i < _countof(SwapChainBuffer); ++i)
			{
				if (!SwapChainBuffer[i])
				{
					SwapChainBuffer[i] = new D3D12RenderTargetResource();

					GetParent()->CreateRenderTargetView(SwapChainBuffer[i], nullptr, rtvHeapHandle);
					rtvHeapHandle.Offset(1, (UINT)RenderTargetViewDesc->GetSize());
				}
			}
		}
	}
}

void D3D12Viewport::OnResize()
{
	assert(SwapChain);
	assert(RenderTargetViewDesc);

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

	CreateBuffer();

	if (DepthStencilBuffer)
		DepthStencilBuffer->Reset();

	DepthStencilBuffer = new D3D12DepthStencilResource();
}

void D3D12Viewport::Create()
{
	assert(GetParent()->GetWindowHandle());

	// Release the previous swapchain we will be recreating.
	SwapChain.Reset();

	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		swapChainDesc.BufferDesc.Width = (UINT)ScreenViewport.Width;
		swapChainDesc.BufferDesc.Height = (UINT)ScreenViewport.Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = BackBufferFormat;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = IsMsaa4xState ? 4 : 1;
		swapChainDesc.SampleDesc.Quality = IsMsaa4xState ? (Msaa4xQuality - 1) : 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = SwapChainBufferCount;
		swapChainDesc.OutputWindow = GetParent()->GetWindowHandle();
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		// Note: Swap chain uses queue to perform flush.
		GetParent()->CreateSwapChain(this, swapChainDesc);
	}
	{
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
	}

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