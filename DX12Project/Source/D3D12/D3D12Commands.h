#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <comdef.h>
#include "d3dx12.h"

#include "CommandList.h"
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


class D3D12CommandList : public RHICommandList
{
public:
	D3D12CommandList() = default;
	virtual ~D3D12CommandList() = default;

	constexpr ComPtr<ID3D12GraphicsCommandList>& Get()
	{
		return CommandList;
	}
	
	ID3D12GraphicsCommandList* GetGraphicsInterface() const
	{
		return CommandList.Get();
	}

	ID3D12CommandList* GetCommandLists();

	void Initialize(D3D12Device* InDevice);

	void BeginDrawWindow(RHIViewport* InViewport) final override;
	void EndDrawWindow(RHIViewport* InViewport) final override;
	void BeginRender() final override;
	void EndRender() final override;

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

	//D3D12PipelineStateCache StateCache;
	
	// const buffer
	// caches : 모든 렌더 상태를 저장(캐싱)하는 구조체
	
	// rhi : 각 기능(struct)을 가져오는 인터페이스(sampler state, pixel shader 등)
	// allocate manager
	// uniform buffers
	//
	// commandlist는 쓰레드마다 진행이 될거고, 여기에 저장된 state cache와 descriptor cache로 장면을 그려나갈거임
	//
};

class D3D12CommandListExecutor : public RHICommandListExecutor
{
public:
	D3D12CommandListExecutor() = default;
	virtual ~D3D12CommandListExecutor() = default;

	void Initialize(D3D12Device* InDevice);

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
