#pragma once
#include "MathHelper.h"
#include "D3D12Commands.h"
#include "D3D12SwapChain.h"

using namespace DirectX;

// Management for command lists
class D3D12RenderInterface
{
public:
	D3D12RenderInterface() = delete;
	D3D12RenderInterface(class D3D12Device* InDevice);
	virtual ~D3D12RenderInterface();

	class D3D12CommandList* GetCommnadList()
	{
		return CommandList.get();
	}

	class D3D12CommandListExecutor* GetCommandExecutor()
	{
		return CmdListExecutor.get();
	}

	//class D3D12Device* GetDevice()
	//{
	//	return Device;
	//}
	
	void ExecuteCommandList();
	void FlushCommandQueue() const;
	void UpdateViewport(class D3D12SwapChain* InSwapChain);
	void SetViewport(class D3DViewportResource& InViewResource, class D3D12SwapChain* InSwapChain);
	void ReadyToRenderTarget(class D3D12Resource* InBackBuffer, D3D12_CPU_DESCRIPTOR_HANDLE InBackBufferView, D3D12_CPU_DESCRIPTOR_HANDLE InDepthStencilBuffer);
	void FinishToRenderTarget(class D3D12Resource* InBackBuffer);
	void ResetCommandList();



private:
	std::unique_ptr<class D3D12CommandListExecutor> CmdListExecutor;
	std::unique_ptr<class D3D12CommandList> CommandList;

	class D3D12Fence* Fence = nullptr;

	std::vector<class D3D12PipelineState*> PipelineStates;
};