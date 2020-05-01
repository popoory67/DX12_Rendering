#include "stdafx.h"
#include "D3D12SwapChain.h"
#include "D3DUtil.h"
#include "d3dx12.h"
#include "D3D12Resource.h"
#include "D3D12Descriptor.h"
#include "D3D12Commands.h"
#include "D3D12RenderInterface.h"

D3D12_VIEWPORT D3D12SwapChain::ScreenViewport;

bool D3D12SwapChain::IsMsaa4xState = false;
UINT D3D12SwapChain::Msaa4xQuality = 0;

D3D12SwapChain::D3D12SwapChain(D3D12DeviceChild* InDevice)
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
}

void D3D12SwapChain::CreateBuffer()
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
					SwapChainBuffer[i] = new D3D12RenderTargetResource(this, rtvHeapHandle, RenderTargetViewDesc->GetSize(), i);
				}
			}
		}
	}
}

void D3D12SwapChain::OnResize()
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
}

D3D12Resource* D3D12SwapChain::GetCurrentBackBuffer() const
{
	if (SwapChainBuffer[CurBackBufferIndex])
	{
		return SwapChainBuffer[CurBackBufferIndex];
	}
	return nullptr;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12SwapChain::GetCurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		RenderTargetViewDesc->GetHeap()->GetCPUDescriptorHandleForHeapStart(),
		CurBackBufferIndex,
		RenderTargetViewDesc->GetSize());
}

void D3D12SwapChain::Create(D3D12CommandListExecutor* InExecutor)
{
	assert(GetParent()->GetWindowHandle());
	assert(InExecutor);

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
		GetParent()->CreateSwapChain(InExecutor, this, swapChainDesc);
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

void D3D12SwapChain::SwapBackBufferToFrontBuffer()
{
	// Swap the back and front buffers
	ThrowIfFailed(SwapChain->Present(0, 0)); // ÀüÈ¯
	CurBackBufferIndex = (CurBackBufferIndex + 1) % SwapChainBufferCount;

	// Advance the fence value to mark commands up to this fence point.
//	CurFrameResource->Fence = ++CurrentFenceCount;
}

float D3D12SwapChain::AspectRatio()const
{
	return static_cast<float>(ScreenViewport.Width) / ScreenViewport.Height;
}