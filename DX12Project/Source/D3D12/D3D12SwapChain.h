#pragma once
#include <dxgi.h>
#include <d3d12.h>
#include "D3DUtil.h"

using namespace Microsoft::WRL;

class D3D12SwapChain
{
public:
	D3D12SwapChain() = delete;
	D3D12SwapChain(class D3D12Device* InDevice);
	virtual ~D3D12SwapChain() {}

	ComPtr<IDXGISwapChain>& Get() { return SwapChain; }

	D3D12_VIEWPORT& GetViewport() { return ScreenViewport; }
	D3D12_RECT& GetRect() { return ScissorRect; }
	static float GetWidth() { return ScreenViewport.Width; }
	static float GetHeight() { return ScreenViewport.Height; }
	static unsigned int GetMsaaQuality() { return Msaa4xQuality; }
	static bool IsMsaa4xEnabled() { return IsMsaa4xState; }
	DXGI_FORMAT GetBackBufferFormat() { return BackBufferFormat; }

	void OnResize(class D3D12Device* InDevice);
	class D3D12Resource* GetCurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
	unsigned int GetSwapChainBufferCount() { return SwapChainBufferCount; }

	void Create(class D3D12Device* InDevice, class D3D12CommandListExecutor* InExecutor);
	void SwapBackBufferToFrontBuffer();

	float AspectRatio() const;

private:
	void CreateBuffer(class D3D12Device* InDevice);

private:
	// 화면을 표시하는 전면/후면 버퍼를 전환해주는 인터페이스(이중 버퍼링)
	ComPtr<IDXGISwapChain> SwapChain = nullptr;

	static bool IsMsaa4xState;		// 4X MSAA enabled
	static UINT Msaa4xQuality;			// quality level of 4X MSAA

	static D3D12_VIEWPORT ScreenViewport;
	D3D12_RECT ScissorRect;

	// Buffers
	static const unsigned int SwapChainBufferCount = 2;
	class D3D12RenderTargetResource* SwapChainBuffer[SwapChainBufferCount];

	int CurBackBufferIndex = 0;

	DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	// descriptor
	class D3D12Descriptor* RenderTargetViewDesc = nullptr;
};