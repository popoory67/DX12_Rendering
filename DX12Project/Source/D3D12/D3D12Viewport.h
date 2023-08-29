#pragma once
#include <dxgi1_5.h>
#include "RenderInterface.h"
#include "D3D12Device.h"
#include "D3D12Types.h"

struct D3DViewportResource
{
public:
	FLOAT TopLeftX;
	FLOAT TopLeftY;
	FLOAT Width;
	FLOAT Height;
	FLOAT MinDepth;
	FLOAT MaxDepth;
};

class D3D12Resource;
class D3D12Descriptor;
class D3D12Fence;

class D3D12Viewport : public D3D12Api, public RHIViewport
{
public:
	D3D12Viewport() = delete;
	explicit D3D12Viewport(D3D12Device* InDevice, HWND InHandle, unsigned int InWidth, unsigned int InHeight);
	virtual ~D3D12Viewport();

	ComPtr<IDXGISwapChain4>& Get() { return SwapChain; }

	D3D12_VIEWPORT& GetViewport() { return ScreenViewport; }
	D3D12_RECT& GetRect() { return ScissorRect; }
	float GetWidth() { return ScreenViewport.Width; }
	float GetHeight() { return ScreenViewport.Height; }

	static unsigned int GetMsaaQuality() { return Msaa4xQuality; }
	static bool IsMsaa4xEnabled() { return IsMsaa4xState; }

	DXGI_FORMAT GetBackBufferFormat() { return BackBufferFormat; }

	void GetRenderTargetView(class D3D12RenderTargetView*& OutRenderTargets) const;

	FORCEINLINE unsigned int GetSwapChainBufferCount() const { return SwapChainBufferCount; }
	//D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilBufferView() const;
	//DXGI_FORMAT GetDepthStencilFormat() const;

	constexpr float GetAspectRatio();

	void Resize();
	void Present();

	void SetViewport(D3DViewportResource& InViewResource);

private:
	D3D12Resource* GetCurrentBackBuffer();

	void CreateSwapChain();
	void CreateSwapChainBuffer();
	//void CreateDepthStencilBuffer();

	void WaitForFrameCompletion();
	void EndFrame();

private:
	HWND WindowHandle = nullptr;

	ComPtr<IDXGISwapChain4> SwapChain = nullptr;

	static bool IsMsaa4xState;
	static UINT Msaa4xQuality;

	D3D12_VIEWPORT ScreenViewport;
	D3D12_RECT ScissorRect;

	// Swap chain
	DXGI_FORMAT BackBufferFormat;

	static const unsigned int SwapChainBufferCount = 2;
	int CurBackBufferIndex = 0;

	class D3D12RenderTargetView* SwapChainBuffer[SwapChainBufferCount];

	// Depth/Stencil
	//DXGI_FORMAT DepthStencilFormat;

	//std::shared_ptr<D3D12Resource> DepthStencilBuffer;
	//std::shared_ptr<D3D12Descriptor> DepthStencilBufferDescriptor;

	D3D12Fence* Fence[SwapChainBufferCount];
	UINT64 CurrentFence;
};

template<>
struct TD3D12Types<RHIViewport>
{
	using ResourceType = D3D12Viewport;
};