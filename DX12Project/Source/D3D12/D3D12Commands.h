#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <comdef.h>
#include <optional>
#include "d3dx12.h"
#include "Util.h"
#include "CommandList.h"
#include "D3D12Device.h"
#include "D3D12Types.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

enum class RenderType : int
{
	Primitive = 0,
	Material,

	Max
};

class D3D12Device;
class D3D12Resource;
class D3D12Fence;
class RHIViewport;

class D3D12CommandAllocator
{
public:
	D3D12CommandAllocator() = delete;
	explicit D3D12CommandAllocator(D3D12Device* InDevice, const D3D12_COMMAND_LIST_TYPE& InType);
	~D3D12CommandAllocator();

	ID3D12CommandAllocator* Get();
	void Reset();

	void Init(D3D12Device* InDevice, const D3D12_COMMAND_LIST_TYPE& InType);

private:
	ComPtr<ID3D12CommandAllocator> CommandAllocator = nullptr;
};


class D3D12CommandList : public RHICommandList, public D3D12Api
{
public:
	D3D12CommandList() = delete;
	explicit D3D12CommandList(D3D12Device* InDevice);
	virtual ~D3D12CommandList();

	constexpr ComPtr<ID3D12GraphicsCommandList>& Get()
	{
		return CommandList;
	}
	
	ID3D12GraphicsCommandList* GetGraphicsInterface() const
	{
		return CommandList.Get();
	}

	ID3D12CommandList* GetCommandLists();

	void Initialize();

	void BeginDrawWindow(RHIViewport* InViewport) final override;
	void EndDrawWindow(RHIViewport* InViewport) final override;
	void BeginRender() final override;
	void EndRender() final override;
	void AddCommand(struct RHICommand*&& InCommand) override;

	FORCEINLINE bool IsClosed() const { return bClosed; }
	void Close();
	void Reset();

	// Indicate a state transition on the resource usage.
	void AddTransition(std::shared_ptr<D3D12Resource> InResource, const D3D12_RESOURCE_STATES& InAfterState);
	void FlushTransitions();

	void ClearRenderTargetView(std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> InDescriptorHandle, XMVECTORF32 InBackColor, UINT InNumRects, const D3D12_RECT* InRect = nullptr);
	void ClearDepthStencilView(std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> InDescriptorHandle, D3D12_CLEAR_FLAGS ClearFlags, float InDepthValue, UINT8 InStencil, UINT InNumRects, const D3D12_RECT* InRect = nullptr);
	void SetRenderTargets(UINT InNumRenderTargetDescriptors, std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> InRenderTargetDescriptorHandle, bool InSingleHandleToDescriptorRange, std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> InDepthStencilDescriptorHandle);
	void SetPipelineState(class std::shared_ptr<class D3D12PipelineState> InPipelineState) const;

	// Primitive
	void SetVertexBuffers(struct D3D12VertexBufferCache& InVertexBufferCache) const;
	void SetIndexBuffer(std::optional<D3D12_INDEX_BUFFER_VIEW> InView = {}) const;
	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY InPrimitiveTopology) const;
	void DrawIndexedInstanced(UINT InIndexCountPerInstance, UINT InInstanceCount, UINT InStartIndexLocation, INT InBaseVertexLocation, UINT InStartInstanceLocation) const;

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

	ComPtr<ID3D12GraphicsCommandList> operator->()
	{
		assert(CommandList && !bClosed);
		return CommandList;
	}

private:
	void CreateCommandList(D3D12Device* InDevice);

private:
	bool bClosed = false;
	ComPtr<ID3D12GraphicsCommandList> CommandList = nullptr; 
	D3D12CommandAllocator* CommandListAllocator = nullptr;

	D3D12Fence* Fence = nullptr;

	std::vector<ID3D12DescriptorHeap*> Heaps;

	std::vector<D3D12_RESOURCE_BARRIER> Barriers;

	std::vector<std::unique_ptr<RHICommand>> Commands;

	//D3D12PipelineStateCache StateCache;
};

class D3D12CommandListExecutor : public RHICommandListExecutor, public D3D12Api
{
public:
	D3D12CommandListExecutor() = delete;
	explicit D3D12CommandListExecutor(D3D12Device* InDevice);
	virtual ~D3D12CommandListExecutor();

	void Initialize();

	void ExecuteCommandLists(RHICommandList* InCommandList) override;
	void FlushCommandLists() override;

	ID3D12CommandQueue* GetCommandQueue() { ReturnCheckAssert(CommandQueue.Get()); }

private:
	void CreateCommandQueue(D3D12Device* InDevice);

private:
	ComPtr<ID3D12CommandQueue> CommandQueue;

	D3D12Fence* Fence = nullptr;
};

template<>
struct TD3D12Types<RHICommandList>
{
	using ResourceType = D3D12CommandList;
};

// struct D3D12ViewportCommand : public CommandBase<D3D12ViewportCommand>
// {
// 	D3D12ViewportCommand() {}
// 	virtual ~D3D12ViewportCommand() {}
// 
// 	virtual void Execute(CommandList& InCmdList)
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
