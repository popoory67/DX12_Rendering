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
#include "D3D12PipelineState.h"

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

class RHIResourceManager : public Uncopyable
{
public:
	RHIResourceManager() = default;
	~RHIResourceManager()
	{
		CleanUp();
	}

	void AddResource(class RHIResource* InResource)
	{
		GpuResources.emplace_back(InResource);
	}

	void CleanUp()
	{
		for (auto resource : GpuResources)
		{
			resource->Reset();
			SafeDelete(resource);
		}
		GpuResources.clear();
	}

private:
	std::vector<class RHIResource*> GpuResources;
};

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

	ID3D12GraphicsCommandList* GetCommandList();

	D3D12PipelineStateCache& GetStateCache();

	void Initialize();

	void BeginDrawWindow(RHIViewport* InViewport) final override;
	void EndDrawWindow(RHIViewport* InViewport) final override;
	void BeginRender() final override;
	void EndRender() final override;
	void ResizeViewport(RHIViewport* InViewport) final override;
	void SetRenderTargets(class RHIRenderTargetInfo* InRenderTargets, unsigned int InNumRenderTarget, class RHIDepthStencilInfo* InDepthStencil) final override;
	void AddResource(class RHIResource* InResource) final override;
	void SetStreamResource(class RHIResource* InVertexBuffer, const UINT InIndicesSize) final override;
	void SetShaderBinding(struct ShaderBinding& InBinding) final override;
	void AddShaderReference(int InIndex, class RHIResource* InBuffer) final override;
	void DrawPrimitive(unsigned int InNumVertices, unsigned int InNumInstances, unsigned int InStartIndex, unsigned int InStartInstance) final override;
	void DrawIndexedInstanced(unsigned int InNumIndices, unsigned int InNumInstances, unsigned int InStartIndex, int InStartVertex, unsigned int InStartInstance) final override;
	void CopyResourceRegion(class RHIResource* InDestResource, class RHIResource* InSourceResource) final override;
	void SetPipelineState(const GraphicsPipelineState::Key& InKey, const GraphicsPipelineState::PSOStream& InPSOCache) final override;
	void CreateAndAddPipelineState(std::vector<struct ShaderBinding*>&& InShaders) final override;

	FORCEINLINE bool IsClosed() const { return bClosed; }
	void Close();
	void Reset();

	// Indicate a state transition on the resource usage.
	void AddTransition(D3D12Resource* InResource, const D3D12_RESOURCE_STATES& InAfterState);
	void FlushTransitions();

	ComPtr<ID3D12GraphicsCommandList> operator->()
	{
		assert(CommandList && !bClosed);
		return CommandList;
	}

private:
	void CreateCommandList(D3D12Device* InDevice);

	void WaitForFrameCompletion();
	void EndFrame();

private:
	bool bClosed = false;

	D3D12PipelineStateCache PSOCache;

	ComPtr<ID3D12GraphicsCommandList> CommandList = nullptr; 
	D3D12CommandAllocator* CommandListAllocator = nullptr;

	std::vector<D3D12_RESOURCE_BARRIER> Barriers;

	D3D12Fence* Fence;
	UINT64 CurrentFence;

	RHIResourceManager ResourceManager;
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

	ID3D12CommandQueue* GetCommandQueue() { return CommandQueue.Get();}

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