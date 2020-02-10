#pragma once
#include "MathHelper.h"

// test
#include "D3D12Commands.h"
#include "D3D12SwapChain.h"
#include "Object.h"


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

	void DrawRenderItems(class D3D12CommandList* InCommandList);
	void RenderPrimitives(class D3D12CommandList* InCommandList);
	void RenderMaterials(class D3D12CommandList* InCommandList);

private:
	class D3D12Resource* GetCurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilBufferView() const;

private:
	// test
	// Render items divided by PSO.
	std::map<unsigned, class Object*> ObjectItems;
	std::map<unsigned, class Primitive*> PrimitiveItems;
	std::map<unsigned, class Material*> MaterialItems;

	class D3D12Device* Device = nullptr;
	class D3D12SwapChain* SwapChain = nullptr;
	class D3D12CommandListExecutor* CmdListExecutor = nullptr;
	class D3D12DepthStencilResource* DepthStencilBuffer = nullptr;
	class D3D12Fence* Fence = nullptr;
	std::vector<class D3D12PipelineState*> PipelineStates;
};

