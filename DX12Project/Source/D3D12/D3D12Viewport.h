#pragma once
#include <dxgi.h>
#include <d3d12.h>
#include "D3DUtil.h"
#include "D3D12Device.h"

using namespace Microsoft::WRL;

class D3D12Viewport : public D3D12DeviceChild
{
public:
	D3D12Viewport(class D3D12DeviceChild* InDevice);
	virtual ~D3D12Viewport() {}

	ComPtr<IDXGISwapChain>& Get() { return SwapChain; }

	D3D12_VIEWPORT& GetViewport() { return ScreenViewport; }
	D3D12_RECT& GetRect() { return ScissorRect; }
	static float GetWidth() { return ScreenViewport.Width; }
	static float GetHeight() { return ScreenViewport.Height; }
	static unsigned int GetMsaaQuality() { return Msaa4xQuality; }
	static bool IsMsaa4xEnabled() { return IsMsaa4xState; }
	DXGI_FORMAT GetBackBufferFormat() { return BackBufferFormat; }

	class D3D12Resource* GetCurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
	unsigned int GetSwapChainBufferCount() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilBufferView() const;
	DXGI_FORMAT GetDepthStencilFormat() const;

	void SwapBackBufferToFrontBuffer();
	float AspectRatio() const;
	void UpdateViewport();
	void SetViewport(class D3DViewportResource& InViewResource);
	void ReadyToRenderTarget();
	void FinishToRenderTarget();
	void OnResize();

private:
	void CreateSwapChain();
	void CreateSwapChainBuffer();
	void CreateDepthStencilBuffer();

private:
	ComPtr<IDXGISwapChain> SwapChain = nullptr;

	static bool IsMsaa4xState;		// 4X MSAA enabled
	static UINT Msaa4xQuality;			// quality level of 4X MSAA

	static D3D12_VIEWPORT ScreenViewport;
	D3D12_RECT ScissorRect;

	// Swap chain
	DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	static const unsigned int SwapChainBufferCount = 2;
	int CurBackBufferIndex = 0;

	class D3D12Resource* SwapChainBuffer[SwapChainBufferCount];
	class D3D12Descriptor* SwapChainBufferDesc = nullptr;

	// Depth/Stencil
	DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	class D3D12Resource* DepthStencilBuffer = nullptr;
	class D3D12Descriptor* DepthStencilBufferDesc = nullptr;
};