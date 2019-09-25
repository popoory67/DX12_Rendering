#include "stdafx.h"
#include "D3D12SwapChain.h"
#include "D3DUtil.h"
#include "d3dx12.h"
#include "D3D12Device.h"
#include "D3D12Resource.h"
#include "D3D12Descriptor.h"
#include "D3D12Commands.h"

D3D12SwapChain::D3D12SwapChain(D3D12Device* InDevice)
{
	memset(SwapChainBuffer, 0x00, _countof(SwapChainBuffer));

	RenderTargetViewDesc = new D3D12Descriptor(InDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void D3D12SwapChain::OnResize(D3D12Device* InDevice)
{
	// resize
	assert(InDevice && InDevice->GetDevice());
	assert(SwapChain);

	// Release the previous resources we will be recreating.
	for (int i = 0; i < _countof(SwapChainBuffer); ++i)
	{
		if (SwapChainBuffer[i] && SwapChainBuffer[i]->Get())
		{
			SwapChainBuffer[i]->Get().Reset();
		}
	}

	// Resize the swap chain.
	ThrowIfFailed(SwapChain->ResizeBuffers(
		_countof(SwapChainBuffer),
		ClientWidth, 
		ClientHeight,
		BackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	CurBackBufferIndex = 0;

	ComPtr<ID3D12DescriptorHeap> pRenderTargetDesc = RenderTargetViewDesc->GetDescriptor();
	if (pRenderTargetDesc)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(pRenderTargetDesc->GetCPUDescriptorHandleForHeapStart());

		for (UINT i = 0; i < _countof(SwapChainBuffer); i++)
		{
			if (SwapChainBuffer[i])
			{
				ComPtr<class ID3D12Resource> pBuffer = SwapChainBuffer[i]->Get();
				if (pBuffer)
				{
					ThrowIfFailed(SwapChain->GetBuffer(i, IID_PPV_ARGS(&pBuffer)));

					if (InDevice->GetDevice())
					{
						InDevice->GetDevice()->CreateRenderTargetView(pBuffer.Get(), nullptr, rtvHeapHandle);
					}

					rtvHeapHandle.Offset(1, RenderTargetViewDesc->GetSize());
				}
			}
		}
	}
}

ID3D12Resource* D3D12SwapChain::GetCurrentBackBuffer() const
{
	if (SwapChainBuffer[CurBackBufferIndex] && SwapChainBuffer[CurBackBufferIndex]->Get())
	{
		return SwapChainBuffer[CurBackBufferIndex]->Get().Get();
	}

	return nullptr;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12SwapChain::GetCurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		RenderTargetViewDesc->GetDescriptor()->GetCPUDescriptorHandleForHeapStart(),
		CurBackBufferIndex,
		RenderTargetViewDesc->GetSize());
}

void D3D12SwapChain::Create(D3D12Device* InDevice, D3D12CommandListExecutor* InExecutor)
{
	assert(InDevice && InDevice->GetWindowHandle());
	assert(SwapChain);
	assert(InExecutor);

	// Release the previous swapchain we will be recreating.
	SwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc.Width = ClientWidth;
	swapChainDesc.BufferDesc.Height = ClientHeight;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = BackBufferFormat;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = IsMsaa4xState ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = IsMsaa4xState ? (Msaa4xQuality - 1) : 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = SwapChainBufferCount;
	swapChainDesc.OutputWindow = InDevice->GetWindowHandle();
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Note: Swap chain uses queue to perform flush.
	ThrowIfFailed(InDevice->GetDxgi()->CreateSwapChain(
		InExecutor->GetExecutor().Get(),
		&swapChainDesc,
		SwapChain.GetAddressOf()));

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render target formats, 
	// so we only need to check quality support.
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MultiSampleQualityLevels;

	MultiSampleQualityLevels.Format = BackBufferFormat;
	MultiSampleQualityLevels.SampleCount = 4;
	MultiSampleQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	MultiSampleQualityLevels.NumQualityLevels = 0;

	ThrowIfFailed(InDevice->GetDevice()->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&MultiSampleQualityLevels,
		sizeof(MultiSampleQualityLevels)));

	Msaa4xQuality = MultiSampleQualityLevels.NumQualityLevels;

	assert(Msaa4xQuality > 0 && "Unexpected MSAA quality level.");
}

void D3D12SwapChain::SwapChainToFrontBuffer()
{
	// Swap the back and front buffers
	ThrowIfFailed(SwapChain->Present(0, 0)); // ÀüÈ¯
	CurBackBufferIndex = (CurBackBufferIndex + 1) % SwapChainBufferCount;

	// Advance the fence value to mark commands up to this fence point.
//	CurFrameResource->Fence = ++CurrentFenceCount;
}