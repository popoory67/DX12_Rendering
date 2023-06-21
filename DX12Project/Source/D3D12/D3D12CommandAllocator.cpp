#include "stdafx.h"
#include "D3D12Commands.h"
#include "D3D12Device.h"

D3D12CommandAllocator::D3D12CommandAllocator(D3D12Device* InDevice, const D3D12_COMMAND_LIST_TYPE& InType)
{
	Init(InDevice, InType);
}

D3D12CommandAllocator::~D3D12CommandAllocator()
{
	CommandAllocator = nullptr;
}

ID3D12CommandAllocator* D3D12CommandAllocator::Get()
{
	return CommandAllocator.Get();
}

void D3D12CommandAllocator::Reset()
{
	ThrowIfFailed(CommandAllocator->Reset());
}

void D3D12CommandAllocator::Init(D3D12Device* InDevice, const D3D12_COMMAND_LIST_TYPE& InType)
{
	ThrowIfFailed(InDevice->GetDevice()->CreateCommandAllocator(
		InType,
		IID_PPV_ARGS(CommandAllocator.GetAddressOf())));
}