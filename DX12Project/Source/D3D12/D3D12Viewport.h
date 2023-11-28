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

	void GetRenderTargetView(class D3D12RenderTargetView*& OutRenderTargets) const;
	void GetDepthStencilView(class D3D12DepthStencilView*& OutDepthStencil) const;

	FORCEINLINE unsigned int GetSwapChainBufferCount() const { return SwapChainBufferCount; }

	constexpr float GetAspectRatio();

	void Resize();
	void Present(D3D12CommandList& InCommandList);

	void SetViewport(D3DViewportResource& InViewResource);

private:
	D3D12Resource* GetCurrentBackBuffer();

	void CreateSwapChain();
	void CreateSwapChainBuffer();
	void CreateDepthStencilBuffer();

private:
	HWND WindowHandle = nullptr;

	static bool IsMsaa4xState;
	static UINT Msaa4xQuality;

	D3D12_VIEWPORT ScreenViewport;
	D3D12_RECT ScissorRect;

	// Swap chain
	DXGI_FORMAT BackBufferFormat;

	static const unsigned int SwapChainBufferCount = 2;
	int CurBackBufferIndex = 0;

	ComPtr<IDXGISwapChain4> SwapChain = nullptr;
	class D3D12RenderTargetView* SwapChainBuffer[SwapChainBufferCount];

	// Depth/Stencil
	DXGI_FORMAT DepthStencilFormat;  // the size of depth-stencil is important to optimize the program. If the device specification is low such as a mobile device, it would be better to choose lower option.
	class D3D12DepthStencilView* DepthStencilBuffer = nullptr;
};

template<>
struct TD3D12Types<RHIViewport>
{
	using ResourceType = D3D12Viewport;
};