#pragma once
#include <dxgi.h>
#include <d3d12.h>

using namespace Microsoft::WRL;

class D3D12SwapChain
{
public:
	D3D12SwapChain() = delete;
	D3D12SwapChain(class D3D12Device* InDevice/*class D3D12Descriptor* InRenderTarget*/);
	virtual ~D3D12SwapChain() {}

	D3D12_VIEWPORT& GetViewport() { return ScreenViewport; }
	D3D12_RECT& GetRect() { return ScissorRect; }
	unsigned int GetWidth() { return /*ClientWidth*/ScreenViewport.Width; }
	unsigned int GetHeight() { return /*ClientHeight*/ScreenViewport.Height; }
	unsigned int GetMsaaQuality() { return Msaa4xQuality; }
	bool IsMsaa4xEnabled() { return IsMsaa4xState; }

	void OnResize(class D3D12Device* InDevice);
	class D3D12Resource* GetCurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
	unsigned int GetSwapChainBufferCount() { return SwapChainBufferCount; }

	void Create(class D3D12Device* InDevice, class D3D12CommandListExecutor* InExecutor);
	void SwapBackBufferToFrontBuffer();

private:
	// 화면을 표시하는 전면/후면 버퍼를 전환해주는 인터페이스(이중 버퍼링)
	ComPtr<IDXGISwapChain> SwapChain = nullptr;

	int ClientWidth = 800;
	int ClientHeight = 600;
	bool IsMsaa4xState = false;		// 4X MSAA enabled
	UINT Msaa4xQuality = 0;			// quality level of 4X MSAA

	D3D12_VIEWPORT ScreenViewport;
	D3D12_RECT ScissorRect;

	// Buffers
	static const unsigned int SwapChainBufferCount = 2;
	class D3D12Resource* SwapChainBuffer[SwapChainBufferCount];

	int CurBackBufferIndex = 0;

	DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	// descriptor
	class D3D12Descriptor* RenderTargetViewDesc = nullptr;
};