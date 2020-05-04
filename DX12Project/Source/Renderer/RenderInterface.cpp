#include "stdafx.h"
#include "RenderInterface.h"
#include "D3D12Resource.h"

D3D12RenderInterface* RenderInterface::CurrentInterface = nullptr;

RenderInterface::RenderInterface(D3D12RenderInterface* InInterface)
{
	SetInterface(InInterface);
}

RenderInterface::~RenderInterface()
{
}

D3D12RenderInterface* RenderInterface::GetInterface()
{
	assert(CurrentInterface);
	return CurrentInterface;
}

void RenderInterface::SetInterface(D3D12RenderInterface* InInterface)
{
	assert(InInterface);
	CurrentInterface = InInterface;
}

void RenderInterface::CreateResource(D3D12Resource* InResource, std::optional<D3D12_RESOURCE_DESC> InDesc, std::optional<D3D12_CLEAR_VALUE> InValue)
{
	assert(CurrentInterface);
	CurrentInterface->CreateResource(InResource, InDesc.value(), InValue.value());
}

void RenderInterface::CreateResource(D3D12Resource* InResource, UINT64 InByteSize, std::optional<D3D12_CLEAR_VALUE> InValue)
{
	assert(CurrentInterface);
	CurrentInterface->CreateResource(InResource, InByteSize, InValue.value());
}

void RenderInterface::CreateDefaultBuffer(D3D12DefaultResource* InResource, const void* InInitData, UINT64 InByteSize)
{
	assert(CurrentInterface);
	CurrentInterface->CreateDefaultBuffer(InResource, InInitData, InByteSize);
}
