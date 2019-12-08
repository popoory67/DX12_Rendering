#include "stdafx.h"
#include <d3d12.h>
#include "D3DUtil.h"
#include "d3dx12.h"
#include "D3D12Resource.h"
#include "D3D12Device.h"
#include "D3D12Descriptor.h"
#include "D3D12SwapChain.h"

D3D12Resource::D3D12Resource()
{

}

D3D12Resource::D3D12Resource(D3D12Device* InDevice, D3D12_RESOURCE_DESC InDesc, D3D12_CLEAR_VALUE InValue)
{
	CreateResource(InDevice, InDesc, InValue);
}

D3D12Resource::D3D12Resource(D3D12Device* InDevice, UINT64 InByteSize, D3D12_CLEAR_VALUE InValue)
{
	CreateResource(InDevice, InByteSize, InValue);
}

void D3D12Resource::CreateResource(D3D12Device* InDevice, D3D12_RESOURCE_DESC InDesc, D3D12_CLEAR_VALUE InValue)
{
	if (InDevice && InDevice->GetDevice())
	{
		Resource.Reset();

		ThrowIfFailed(InDevice->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&InDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&InValue,
			IID_PPV_ARGS(Resource.GetAddressOf())));
	}
}

void D3D12Resource::CreateResource(D3D12Device* InDevice, UINT64 InByteSize, D3D12_CLEAR_VALUE InValue)
{
	if (InDevice && InDevice->GetDevice())
	{
		ThrowIfFailed(InDevice->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(InByteSize),
			D3D12_RESOURCE_STATE_COMMON,
			&InValue,
			IID_PPV_ARGS(Resource.GetAddressOf())));
	}
}

D3D12RenderTargetResource::D3D12RenderTargetResource(D3D12Device* InDevice, D3D12SwapChain* InSwapChain, CD3DX12_CPU_DESCRIPTOR_HANDLE& IntDescriptorHandle, UINT InDescriptorSize, unsigned int InIndex)
{
	assert(InDevice || InSwapChain);

	if (InSwapChain->Get())
		ThrowIfFailed(InSwapChain->Get()->GetBuffer(InIndex, IID_PPV_ARGS(&Resource)));

	if (InDevice->GetDevice())
		InDevice->GetDevice()->CreateRenderTargetView(Resource.Get(), nullptr, IntDescriptorHandle);

	IntDescriptorHandle.Offset(1, InDescriptorSize);
}

D3D12DepthStencilResource::D3D12DepthStencilResource(D3D12Device* InDevice, D3D12Descriptor* InDescriptor, D3D12_DEPTH_STENCIL_VIEW_DESC InDepthStencilDesc, D3D12_RESOURCE_DESC InDesc, D3D12_CLEAR_VALUE InValue)
{
	assert(InDevice || InDevice->GetDevice());

	CreateResource(InDevice, InDesc, InValue);

	if (InDescriptor)
		InDevice->GetDevice()->CreateDepthStencilView(Resource.Get(), &InDepthStencilDesc, InDescriptor->GetDescriptor()->GetCPUDescriptorHandleForHeapStart());
}