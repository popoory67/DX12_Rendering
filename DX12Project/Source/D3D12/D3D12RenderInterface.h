#pragma once
#include "MathHelper.h"

// test
#include "D3D12Commands.h"
#include "D3D12SwapChain.h"


using namespace DirectX;

class D3D12RenderInterface
{
public:
	D3D12RenderInterface() = delete;

	D3D12RenderInterface(class D3D12Device* InDevice, class D3D12CommandList* InCommandList);
	~D3D12RenderInterface();

	class D3D12Device* GetDevice() { return Device; }
	class D3D12SwapChain* GetSwapChain() { return SwapChain; }

	DXGI_FORMAT GetBackBufferFormat() const;
	DXGI_FORMAT GetDepthStencilFormat() const;
	
	void ExecuteCommandList(class D3D12CommandList* InCommandList);
	void FlushCommandQueue() const;
	void UpdateViewport(class D3D12CommandList* InCommandList);
	void SetViewport(class D3DViewportResource& InViewResource);
	void ReadyToRenderTarget(class D3D12CommandList* InCommandList);
	void FinishToRenderTarget(class D3D12CommandList* InCommandList);
	void SwapBackBufferToFrontBuffer();
	void OnResize(class D3D12CommandList* InCommandList);

protected:
	class D3D12Resource* GetCurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilBufferView() const;

protected:

	class D3D12Device* Device = nullptr; // multi gpu 지원 안함
	class D3D12SwapChain* SwapChain = nullptr;
	class D3D12CommandListExecutor* CmdListExecutor = nullptr;
	class D3D12DepthStencilResource* DepthStencilBuffer = nullptr;
	class D3D12Fence* Fence = nullptr;
	std::vector<class D3D12PipelineState*> PipelineStates;
};

