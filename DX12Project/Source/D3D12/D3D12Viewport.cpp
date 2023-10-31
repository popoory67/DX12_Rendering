#include "D3D12Viewport.h"
#include <DirectXColors.h>
#include "d3dx12.h"
#include "D3D12Resource.h"
#include "D3D12View.h"
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
	, DepthStencilFormat(DXGI_FORMAT_D32_FLOAT)
{
	ScreenViewport.Width = static_cast<float>(InWidth);
	ScreenViewport.Height = static_cast<float>(InHeight);

	CreateSwapChain();
}

D3D12Viewport::~D3D12Viewport()
{
	WindowHandle = nullptr;

	for (int i = 0; i < SwapChainBufferCount; ++i)
	{
		SafeDelete(SwapChainBuffer[i]);
	}
	SafeDelete(DepthStencilBuffer);
}

D3D12Resource* D3D12Viewport::GetCurrentBackBuffer()
{
	if (SwapChainBuffer[CurBackBufferIndex])
	{
		return D3D12RHI::Cast(SwapChainBuffer[CurBackBufferIndex]->GetTexture());
	}
	return nullptr;
}

void D3D12Viewport::GetRenderTargetView(D3D12RenderTargetView*& OutRenderTargets) const
{
    OutRenderTargets = SwapChainBuffer[CurBackBufferIndex];
    //memcpy_s(OutRenderTargets, sizeof(D3D12RenderTargetView*), SwapChainBuffer, sizeof(D3D12RenderTargetView*));
}

void D3D12Viewport::GetDepthStencilView(D3D12DepthStencilView*& OutDepthStencil) const
{
	OutDepthStencil = DepthStencilBuffer;
}

constexpr float D3D12Viewport::GetAspectRatio()
{
	return static_cast<float>(ScreenViewport.Width) / ScreenViewport.Height;
}

void D3D12Viewport::Present(D3D12CommandList& InCommandList)
{
	if (D3D12Resource* backBuffer = GetCurrentBackBuffer())
	{
		InCommandList.AddTransition(backBuffer, D3D12_RESOURCE_STATE_PRESENT);

		// Before a command queue executes command lists,
		// that lists have to be closed. (EXECUTECOMMANDLISTS_OPENCOMMANDLIST)
		InCommandList.Close();

		GetParent()->GetCommandListExecutor().ExecuteCommandLists(&InCommandList);

		// Swap the back and front buffers
		ThrowIfFailed(SwapChain->Present(1, 0));
	}

    CurBackBufferIndex = (CurBackBufferIndex + 1) % SwapChainBufferCount;
}

void D3D12Viewport::Resize()
{
	// Process to resize back buffers that are a render target type.
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
	}
	CreateSwapChainBuffer();

	// Process to resize depth-stencil buffers.
	if (DepthStencilBuffer)
	{
		DepthStencilBuffer->Reset();
	}
	CreateDepthStencilBuffer();
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
	swapChainDesc.SampleDesc.Count = IsMsaa4xEnabled() ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = IsMsaa4xEnabled() ? (Msaa4xQuality - 1) : 0;

	swapChainDesc.BufferCount = SwapChainBufferCount;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = WindowHandle;
	swapChainDesc.Windowed = true;

	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Note: Swapchain uses queue to perform flush.
	if (auto pDevice = GetParent())
	{
		auto commandQueue = pDevice->GetCommandQueue();

		ComPtr<IDXGISwapChain> swapChain;
		ThrowIfFailed(GetParent()->GetDxgi()->CreateSwapChain(commandQueue, &swapChainDesc, &swapChain));

		ThrowIfFailed(swapChain->QueryInterface(IID_PPV_ARGS(SwapChain.GetAddressOf())));

		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MultiSampleQualityLevels;
		MultiSampleQualityLevels.Format = BackBufferFormat;
		MultiSampleQualityLevels.SampleCount = IsMsaa4xEnabled() ? 4 : 1;
		MultiSampleQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		MultiSampleQualityLevels.NumQualityLevels = IsMsaa4xEnabled() ? (Msaa4xQuality - 1) : 0;

		ThrowIfFailed(GetDevice()->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &MultiSampleQualityLevels, sizeof(MultiSampleQualityLevels)));

		Msaa4xQuality = MultiSampleQualityLevels.NumQualityLevels;
	}

	::PostMessage(WindowHandle, WM_PAINT, 0, 0);
}

void D3D12Viewport::CreateSwapChainBuffer()
{
    for (int i = 0; i < SwapChainBufferCount; ++i)
    {
        ComPtr<ID3D12Resource> backBufferResource;
        ThrowIfFailed(SwapChain->GetBuffer(i, IID_PPV_ARGS(backBufferResource.GetAddressOf())));

        D3D12Resource* resource = new D3D12Resource(std::move(backBufferResource));
        resource->SetResourceState(D3D12_RESOURCE_STATE_PRESENT);

        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
        rtvHeapDesc.NumDescriptors = GetSwapChainBufferCount();
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtvHeapDesc.NodeMask = 0;

        D3D12Descriptor* descriptor = new D3D12Descriptor(GetParent(), rtvHeapDesc);

        SwapChainBuffer[i] = new D3D12RenderTargetView(resource, descriptor);

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(SwapChainBuffer[i]->GetHandle());
        GetDevice()->CreateRenderTargetView(resource->GetResource(), nullptr, rtvHandle);
    }
}

void D3D12Viewport::CreateDepthStencilBuffer()
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	
	D3D12Descriptor* descriptor = new D3D12Descriptor(GetParent(), dsvHeapDesc);
	{
		ComPtr<ID3D12Resource> depthStencilBuffer;
		D3D12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

        D3D12_RESOURCE_DESC desc;
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Alignment = 0;
        desc.Width = (UINT64)GetWidth();
        desc.Height = (UINT)GetHeight();
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
		desc.Format = DepthStencilFormat;
        desc.SampleDesc.Count = IsMsaa4xEnabled() ? 4 : 1;
        desc.SampleDesc.Quality = IsMsaa4xEnabled() ? (GetMsaaQuality() - 1) : 0;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE optClear;
        optClear.Format = DepthStencilFormat;
        optClear.DepthStencil.Depth = 1.0f;
        optClear.DepthStencil.Stencil = 0;

		ThrowIfFailed(GetDevice()->CreateCommittedResource(
			&properties,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&optClear,
			IID_PPV_ARGS(&depthStencilBuffer)));

		D3D12Resource* resource = new D3D12Resource(std::move(depthStencilBuffer));
		resource->SetResourceState(D3D12_RESOURCE_STATE_COMMON);
		{
            // Create descriptor to mip level 0 of entire resource using the format of the resource.
            D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
            dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            dsvDesc.Format = DepthStencilFormat;
            dsvDesc.Texture2D.MipSlice = 0;

            GetDevice()->CreateDepthStencilView(resource->GetResource(), &dsvDesc, descriptor->GetCpuHandle(0));
		}

		DepthStencilBuffer = new D3D12DepthStencilView(resource, descriptor);
	}
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

	D3D12_RECT& scissorRect = GetRect();

	scissorRect = { 0, 0, (LONG)viewport.Width, (LONG)viewport.Height };
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
