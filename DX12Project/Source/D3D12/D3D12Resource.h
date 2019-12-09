#pragma once
#include <intsafe.h>

using namespace Microsoft::WRL;

// buffer
class D3D12Resource
{
public:
	D3D12Resource();
	D3D12Resource(class D3D12Device* InDevice, D3D12_RESOURCE_DESC InDesc, D3D12_CLEAR_VALUE InValue);
	D3D12Resource(class D3D12Device* InDevice, UINT64 InByteSize, D3D12_CLEAR_VALUE InValue);
	virtual ~D3D12Resource() {}

	ComPtr<struct ID3D12Resource>& Get() { return Resource; }
	void Reset();

protected:
	void CreateResource(class D3D12Device* InDevice, D3D12_RESOURCE_DESC InDesc, D3D12_CLEAR_VALUE InValue);
	void CreateResource(class D3D12Device* InDevice, UINT64 InByteSize, D3D12_CLEAR_VALUE InValue);

protected:
	ComPtr<struct ID3D12Resource> Resource = nullptr;
};

class D3D12RenderTargetResource : public D3D12Resource
{
public:
	D3D12RenderTargetResource() = delete;
//	D3D12RenderTargetResource(class D3D12Device* InDevice, CD3DX12_CPU_DESCRIPTOR_HANDLE& InDescriptorHandle, UINT InDescriptorSize); // swap에서만 사용하고있기 떄문에 일단은 봉인
	D3D12RenderTargetResource(class D3D12Device* InDevice, class D3D12SwapChain* InSwapChain, CD3DX12_CPU_DESCRIPTOR_HANDLE& InDescriptorHandle, UINT InDescriptorSize, unsigned int InIndex);

	virtual ~D3D12RenderTargetResource() {}
};

class D3D12DepthStencilResource : public D3D12Resource
{
public:
	D3D12DepthStencilResource() = delete;
	D3D12DepthStencilResource(class D3D12Device* InDevice, class D3D12Descriptor* InDescriptor, D3D12_DEPTH_STENCIL_VIEW_DESC& InDepthStencilDesc, D3D12_RESOURCE_DESC& InDesc, D3D12_CLEAR_VALUE InValue);

	virtual ~D3D12DepthStencilResource() {}
};