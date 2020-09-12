#include "stdafx.h"
#include "RenderInterface.h"
#include "D3D12Resource.h"
#include "SceneRendering.h"

RenderInterface::RenderInterface(D3D12RenderInterface* InInterface, SceneRenderer* InSceneRenderer)
{
	assert(InInterface);
	assert(InSceneRenderer);
	CurrentInterface = std::make_unique<D3D12RenderInterface>(InInterface);
	CurrnetSceneRenderer = std::make_unique<SceneRenderer>(InSceneRenderer);
}

void RenderInterface::CreateResource(D3D12Resource* InResource, std::optional<D3D12_RESOURCE_DESC> InDesc, std::optional<D3D12_CLEAR_VALUE> InValue)
{
	CurrentInterface->CreateResource(InResource, InDesc.value(), InValue.value());
}

void RenderInterface::CreateResource(D3D12Resource* InResource, UINT64 InByteSize, std::optional<D3D12_CLEAR_VALUE> InValue)
{
	CurrentInterface->CreateResource(InResource, InByteSize, InValue.value());
}

void RenderInterface::CreateDefaultBuffer(D3D12DefaultResource* InResource, const void* InInitData, UINT64 InByteSize)
{
	CurrentInterface->CreateDefaultBuffer(InResource, InInitData, InByteSize);
}

void RenderInterface::AddScene(Scene* InScene) const
{
	assert(InScene);
	CurrnetSceneRenderer->AddScene(InScene);
}
