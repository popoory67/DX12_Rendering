#include "RenderComponent.h"
#include "D3DUtil.h" // test

RenderComponent::RenderComponent(Scene* InScene, Component* InParent)
	: Parent(InScene, InParent)
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
