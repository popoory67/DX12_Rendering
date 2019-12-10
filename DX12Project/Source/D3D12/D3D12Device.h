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
	D3D12Device(HWND InWindowHandle);
	virtual ~D3D12Device();

	ComPtr<ID3D12Device> Get() { ReturnCheckAssert(Device); }
	ID3D12Device* GetInterface() { assert(Device); ReturnCheckAssert(Device.Get()); }
	ComPtr<IDXGIFactory4> GetDxgi() { ReturnCheckAssert(DxgiFactory); }
	HWND GetWindowHandle() { ReturnCheckAssert(MainWindowHandle); }

	// Command interfaces
	void CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE InCommandListType, ComPtr<ID3D12CommandAllocator>& InCommandAllocator);
	void CreateCommandList(D3D12_COMMAND_LIST_TYPE InCommandListType, ComPtr<ID3D12CommandAllocator>& InCommandAllocator, ComPtr<ID3D12GraphicsCommandList>& InCommandList);
	void CreateCommandQueue(D3D12_COMMAND_QUEUE_DESC& InQueueDesc, ComPtr<ID3D12CommandQueue>& InCommandQueue);

	// Swap chain interfaces
	void CreateSwapChain(class D3D12CommandListExecutor* InExecutor, class D3D12SwapChain* InSwapChain, DXGI_SWAP_CHAIN_DESC& InSwapChainDesc);
	void CheckFeatureSupport(D3D12_FEATURE InFeature, D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS& InMultisampleQualityLevels);

	// Resource interfaces
	void CreateCommittedResource(ComPtr<ID3D12Resource>& InResource, D3D12_HEAP_TYPE InHeapType, D3D12_HEAP_FLAGS InHeapFlags, D3D12_RESOURCE_DESC& InDesc, D3D12_RESOURCE_STATES InResourceStates, D3D12_CLEAR_VALUE InValue);
	void CreateRenderTargetView(ComPtr<ID3D12Resource>& InResource, const D3D12_RENDER_TARGET_VIEW_DESC* InDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE& InDescriptorHandle);
	void CreateDepthStencilView(ComPtr<ID3D12Resource>& InResource, class D3D12Descriptor* InDescriptor, D3D12_DEPTH_STENCIL_VIEW_DESC& InDepthStencilDesc);
	void CreateShaderView(ComPtr<ID3D12Resource>& InResource, class D3D12Descriptor* InDescriptor, D3D12_SHADER_RESOURCE_VIEW_DESC& InShaderDesc);

	// Descriptor interfaces
	UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE InDescriptorHeapType);

	// Pipeline state interfaces
	void CreateGraphicsPipelineState(ComPtr<ID3D12PipelineState> InPipelineState, D3D12_GRAPHICS_PIPELINE_STATE_DESC& InPipelineStateDesc);

private:
	void CreateDevice();

private:
	// COM(Component Object Model)
	// The COM object is like a C++ class.
	// DX의 프로그래밍 언어 독립성과 하위 호환성을 가능하게 하는 기술

	// 언어 독립성 : C++을 C++로써만 사용할 수 있게 함
	// 하위 호환성 : 내부와 연결하여 사용할 수 있게 함

	// 말하자면, Com 인터페이스를 하나의 클래스 오브젝트로 생각하고 사용하면 된다는 의미

	// ComPtr is like a smart pointer.
	ComPtr<ID3D12Device> Device; // Render를 위한 기능을 담고 있는 인터페이스

	// DirectX Graphics Infrastructure : 공통 그래픽 API
	ComPtr<IDXGIFactory4> DxgiFactory; // Swap chain 인터페이스 생성, 디스플레이 어댑터(= 그래픽카드) 정보

	HWND MainWindowHandle = nullptr;
};