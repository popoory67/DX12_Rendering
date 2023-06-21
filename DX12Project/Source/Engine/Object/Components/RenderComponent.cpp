#include "stdafx.h"
#include "RenderComponent.h"
#include "D3DUtil.h"

UINT RenderComponent::GetBindingSize()
{
	size_t size = 0;
	for (auto it = Bindings.cbegin(); it != Bindings.cend(); ++it)
	{
		size += sizeof(it->second);
	}

	return D3DUtil::CalcConstantBufferByteSize((UINT)size);
}