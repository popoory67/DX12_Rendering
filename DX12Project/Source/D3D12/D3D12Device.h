#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include "d3dx12.h"
#include "MathHelper.h"
#include "D3DUtil.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class D3DDeviceBase
{
public:
	D3DDeviceBase() {}
	virtual ~D3DDeviceBase() {}
};

class D3D12Device : public D3DDeviceBase
{
public:
	D3D12Device();
	virtual ~D3D12Device();

	ComPtr<ID3D12Device> GetDevice() const 
	{ 
		ReturnCheckAssert(Device); 
	}

	ID3D12Device* GetDeviceInterface() const
	{ 
		assert(Device); ReturnCheckAssert(Device.Get());
	}

	ComPtr<IDXGIFactory4> GetDxgi() const 
	{
		ReturnCheckAssert(DxgiFactory);
	}

	HWND GetWindowHandle() 
	{ 
		ReturnCheckAssert(MainWindowHandle); 
	}

	class D3D12CommandList* GetCommandList()
	{
		ReturnCheckAssert(CommandList.get());
	}

	void ExecuteCommands();
	void FlushCommandQueue();

	// Command interfaces
	void CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE InCommandListType, ComPtr<ID3D12CommandAllocator>& InCommandAllocator);
	void CreateCommandList(D3D12_COMMAND_LIST_TYPE InCommandListType, ComPtr<ID3D12CommandAllocator>& InCommandAllocator, ComPtr<ID3D12GraphicsCommandList>& InCommandList);
	void CreateCommandQueue(D3D12_COMMAND_QUEUE_DESC& InQueueDesc, ComPtr<ID3D12CommandQueue>& InCommandQueue);

	// Swap chain interfaces
	void CreateSwapChain(ComPtr<IDXGISwapChain>& InSwapChain, DXGI_SWAP_CHAIN_DESC& InSwapChainDesc);
	void CheckFeatureSupport(D3D12_FEATURE InFeature, D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS& InMultisampleQualityLevels);

	// Resource interfaces
	void CreateCommittedResource(class D3D12Resource* InResource, D3D12_HEAP_TYPE InHeapType, D3D12_HEAP_FLAGS InHeapFlags, D3D12_RESOURCE_DESC& InDesc, D3D12_RESOURCE_STATES InResourceStates, std::optional<D3D12_CLEAR_VALUE> InValue = {});
	void CreateCommittedResource(class D3D12Resource* InResource, D3D12_HEAP_TYPE InHeapType, D3D12_HEAP_FLAGS InHeapFlags, UINT64 InByteSize, D3D12_RESOURCE_STATES InResourceStates, std::optional<D3D12_CLEAR_VALUE> InValue = {});
	void CreateRenderTargetView(class D3D12Resource* InResource, const D3D12_RENDER_TARGET_VIEW_DESC* InDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE& InDescriptorHandle);
	void CreateDepthStencilView(class D3D12Resource* InResource, class D3D12Descriptor* InDescriptor, D3D12_DEPTH_STENCIL_VIEW_DESC& InDepthStencilDesc);
	void CreateShaderView(class D3D12Resource* InResource, class D3D12Descriptor* InDescriptor, D3D12_SHADER_RESOURCE_VIEW_DESC& InShaderDesc);

	// Descriptor interfaces
	void CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_DESC& InHeapDesc, ComPtr<ID3D12DescriptorHeap>& InHeap);
	UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE InDescriptorHeapType);

	// Pipeline state interfaces
	void CreateGraphicsPipelineState(ComPtr<ID3D12PipelineState>& InPipelineState, D3D12_GRAPHICS_PIPELINE_STATE_DESC& InPipelineStateDesc);

	// Root signature interfaces
	void CreateRootSignature(class D3D12RootSignature* InRootSignature, class D3D12BinaryLargeObject* InBlob);

private:
	void CreateDevice();

protected:
	ComPtr<ID3D12Device> Device = nullptr; 

	// DirectX Graphics Infrastructure : 공통 그래픽 API
	ComPtr<IDXGIFactory4> DxgiFactory = nullptr;

	std::unique_ptr<class D3D12CommandListExecutor> CommandListExecutor;
	std::unique_ptr<class D3D12CommandList> CommandList;

	HWND MainWindowHandle = nullptr;
};

class D3D12DeviceChild
{
public:
	D3D12DeviceChild(D3D12Device* InParent = nullptr) : Parent(InParent) {}

	D3D12Device* GetParent() const
	{
		ReturnCheckAssert(Parent);
	}

private:
	D3D12Device* Parent = nullptr;
};