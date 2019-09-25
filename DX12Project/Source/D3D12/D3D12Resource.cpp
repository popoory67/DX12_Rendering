#include "stdafx.h"
#include <d3d12.h>
#include "D3DUtil.h"
#include "d3dx12.h"
#include "D3D12Resource.h"
#include "D3D12Device.h"

void D3D12Resource::CreateResource(D3D12Device* InDevice, D3D12_RESOURCE_DESC* InDesc/* = nullptr */, D3D12_CLEAR_VALUE* InValue/* = nullptr */)
{
	if (InDevice && InDevice->GetDevice() && InDesc)
	{
		ThrowIfFailed(InDevice->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			InDesc,
			D3D12_RESOURCE_STATE_COMMON,
			InValue,
			IID_PPV_ARGS(Resource.GetAddressOf())));
	}
}

void D3D12Resource::CreateResource(D3D12Device* InDevice, UINT64 InByteSize, D3D12_CLEAR_VALUE* InValue/* = nullptr */)
{
	if (InDevice && InDevice->GetDevice())
	{
		ThrowIfFailed(InDevice->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(InByteSize),
			D3D12_RESOURCE_STATE_COMMON,
			InValue,
			IID_PPV_ARGS(Resource.GetAddressOf())));
	}
}