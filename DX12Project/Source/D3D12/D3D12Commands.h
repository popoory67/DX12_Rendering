#pragma once
#include "CommnadList.h"
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <stdint.h>
#include <comdef.h>
#include "d3dx12.h"
#include <DirectXColors.h>
#include "D3DUtil.h"
#include "GameTimer.h"
#include "FrameResource.h"
#include "D3D12Resource.h"
#include "D3D12Descriptor.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

enum class RenderType : UINT
{
	Primitive,
	Material,

	Max
};

class D3D12CommandList : public CommandListBase
{
public:
	D3D12CommandList() = delete;
	D3D12CommandList(class D3D12Device* InD3D12Device);

	ComPtr<ID3D12GraphicsCommandList>& Get() { return CommandList; }
	ID3D12GraphicsCommandList* GetGraphics() { return CommandList.Get(); }
	ID3D12CommandList* GetCommandLists();

	// Indicate a state transition on the resource usage.
	void SetResourceTransition(class D3D12Resource* InResource, D3D12_RESOURCE_STATES InPrevState, D3D12_RESOURCE_STATES InNextState);
	void ResourceBarrier(class D3D12Resource* InResource, D3D12_RESOURCE_STATES InFrom, D3D12_RESOURCE_STATES InTo);

	template <UINT MaxResources>
	void UpdateResources(class D3D12Resource* InFromResource, class D3D12Resource* InToResource, UINT InFirstSubresource, UINT InNumSubresources, UINT64 InRequiredSize, std::optional<D3D12_SUBRESOURCE_DATA> InSubresourceData = {});

	void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE InDescriptorHandle, XMVECTORF32 InBackColor, UINT InNumRects, const D3D12_RECT* InRect = nullptr);
	void ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE InDescriptorHandle, D3D12_CLEAR_FLAGS ClearFlags, float InDepthValue, UINT8 InStencil, UINT InNumRects, const D3D12_RECT* InRect = nullptr);
	void SetRenderTargets(UINT InNumRenderTargetDescriptors, D3D12_CPU_DESCRIPTOR_HANDLE InRenderTargetDescriptorHandle, bool InSingleHandleToDescriptorRange, D3D12_CPU_DESCRIPTOR_HANDLE InDepthStencilDescriptorHandle);
	void SetPipelineState(class D3D12PipelineState* InPipelineState);

	// Primitive
	void SetVertexBuffers(UINT InStartSlot, UINT InNumViews, std::optional<D3D12_VERTEX_BUFFER_VIEW> InViews = {});
	void SetIndexBuffer(std::optional<D3D12_INDEX_BUFFER_VIEW> InView = {});
	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY InPrimitiveTopology);
	void DrawIndexedInstanced(UINT InIndexCountPerInstance, UINT InInstanceCount, UINT InStartIndexLocation, INT InBaseVertexLocation, UINT InStartInstanceLocation);

	// Descriptor heap
	void AddDescriptorHeap(class D3D12Descriptor* InDescriptor);
	void ExecuteHeaps();
	void FlushHeaps();

	// Root signature
	void SetRootSignature(class D3D12RootSignature* InRootSignature);
	void BindTable(RenderType InRenderType, CD3DX12_GPU_DESCRIPTOR_HANDLE InDescriptorHandle);
	void BindConstBuffer(RenderType InRenderType, std::vector<D3D12_GPU_VIRTUAL_ADDRESS>& InAddresses);
	void BindConstBuffer(RenderType InRenderType, D3D12_GPU_VIRTUAL_ADDRESS& InAddress);
	void BindShaderResource(RenderType InRenderType, std::vector<D3D12_GPU_VIRTUAL_ADDRESS>& InAddresses);
	void BindShaderResource(RenderType InRenderType, D3D12_GPU_VIRTUAL_ADDRESS& InAddress);

	void Reset();

	// 원래는 base에 있어야하는데 D3D12용 인터페이스라서 일단 여기에 둠1
protected:
// 	ComPtr<ID3D12DescriptorHeap> DescriptorHeap = nullptr;
// 	ComPtr<ID3D12RootSignature> RootSignature = nullptr;

private:
	ComPtr<ID3D12GraphicsCommandList> CommandList = nullptr;
	ComPtr<ID3D12CommandAllocator> CommandListAllocator = nullptr;

	std::vector<ID3D12DescriptorHeap*> Heaps;
};

class D3D12CommandListExecutor : public CommandListExecutor
{
public:
	D3D12CommandListExecutor() = delete;
	D3D12CommandListExecutor(class D3D12Device* InD3D12Device);

	void Execute(CommandListBase& InCommandList) override {}
	void Execute(class D3D12CommandList* InCommandList);
	void FlushCommands() override;

	ComPtr<ID3D12CommandQueue>& GetExecutor() { ReturnCheckAssert(CommandQueue); }
	ID3D12CommandQueue* GetExecutorInterface() { ReturnCheckAssert(CommandQueue.Get()); }

private:
	ComPtr<ID3D12CommandQueue> CommandQueue = nullptr;
	//ComPtr<ID3D12Fence> Fence;
	class D3D12Fence* Fence = nullptr;

	UINT64 CurrentFenceCount = 0;
};

// 여기부터 command
// struct D3D12ViewportCommand : public CommandBase<D3D12ViewportCommand>
// {
// 	D3D12ViewportCommand() {}
// 	virtual ~D3D12ViewportCommand() {}
// 
// 	virtual void Execute(CommandListBase& InCmdList)
// 	{
// 		D3D12CommandList* pList = static_cast<class D3D12CommandList*>(&InCmdList);
// 		if (pList)
// 		{
// 			ComPtr<ID3D12GraphicsCommandList> pD3d12CommantList = pList->Get();
// 
// 			pD3d12CommantList->RSSetViewports(1, &ScreenViewport);
// 		}
// 	}
// 
// private:
// 	D3D12_VIEWPORT ScreenViewport;
// };
