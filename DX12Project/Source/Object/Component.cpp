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

UINT RenderComponent::GetBindingSize()
{
	size_t size = 0;
	for (auto it = Bindings.cbegin(); it != Bindings.cend(); ++it)
	{
		size += sizeof(it->second);
	}

	return D3DUtil::CalcConstantBufferByteSize((UINT)size);
}