#include "stdafx.h"
#include "Component.h"
#include "D3DUtil.h"

Component::Component()
{

}

Component::~Component()
{

}

RenderComponent::RenderComponent()
{
}

RenderComponent::~RenderComponent()
{
}

UINT RenderComponent::GetBatchSize()
{
	size_t size = 0;
	for (auto it : Batches)
	{
		size += it.second.dataType;
	}

	return D3DUtil::CalcConstantBufferByteSize((UINT)size);
}