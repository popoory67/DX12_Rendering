#pragma once
#include "CommnadList.h"
#include <d3d12.h>
#include <DirectXMath.h>
#include <comdef.h>
#include "d3dx12.h"
#include "D3DUtil.h"
#include "D3D12Resource.h"
#include "D3D12PipelineState.h"

#pragma comment(lib,"d3dcompiler.lib")
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

class D3D12Resource;

class D3D12CommandList : public CommandListBase
{
public:
	D3D12CommandList() = delete;
	D3D12CommandList(class D3D12Device* InDevice);

	virtual ~D3D12CommandList() {}

	ComPtr<ID3D12GraphicsCommandList>& Get() 
	{
		ReturnCheckAssert(CommandList);
	}
	
	ID3D12GraphicsCommandList* GetGraphicsInterface() const
	{
		ReturnCheckAssert(CommandList.Get());
	}

	ID3D12CommandList* GetCommandLists();

	// Indicate a state transition on the resource usage.
	void SetResourceTransition(class D3D12Resource* InResource, D3D12_RESOURCE_STATES InPrevState, D3D12_RESOURCE_STATES InNextState);
	void ResourceBarrier(class D3D12Resource* InResource, D3D12_RESOURCE_STATES InFrom, D3D12_RESOURCE_STATES InTo);

	void ClearRenderTargetView(std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> InDescriptorHandle, XMVECTORF32 InBackColor, UINT InNumRects, const D3D12_RECT* InRect = nullptr);
	void ClearDepthStencilView(std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> InDescriptorHandle, D3D12_CLEAR_FLAGS ClearFlags, float InDepthValue, UINT8 InStencil, UINT InNumRects, const D3D12_RECT* InRect = nullptr);
	void SetRenderTargets(UINT InNumRenderTargetDescriptors, std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> InRenderTargetDescriptorHandle, bool InSingleHandleToDescriptorRange, std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> InDepthStencilDescriptorHandle);
	void SetPipelineState(class D3D12PipelineState* InPipelineState) const;

	// Primitive
	/// Draw �Ǵ� ������ �Ҹ���
	/// StreamSource(�����͸� �ִ� ����), Set(Draw call ����)
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

	void Reset();

private:
	ComPtr<ID3D12GraphicsCommandList> CommandList = nullptr; // List manager�� �����ϰ� �� command list�� ������� ���� ��Ƽ�� ������ �� �ְ� �ؾ���
	ComPtr<ID3D12CommandAllocator> CommandListAllocator = nullptr;

	std::vector<ID3D12DescriptorHeap*> Heaps;

	D3D12PipelineStateCache StateCache;
	
	// const buffer
	// caches : ��� ���� ���¸� ����(ĳ��)�ϴ� ����ü
	
	// rhi : �� ���(struct)�� �������� �������̽�(sampler state, pixel shader ��)
	// allocate manager
	// uniform buffers
	//
	// commandlist�� �����帶�� ������ �ɰŰ�, ���⿡ ����� state cache�� descriptor cache�� ����� �׷���������
	//
};

class D3D12CommandListExecutor : public CommandListExecutor
{
public:
	D3D12CommandListExecutor() = delete;
	D3D12CommandListExecutor(class D3D12Device* InDevice);
	virtual ~D3D12CommandListExecutor();

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

// ������� command
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
