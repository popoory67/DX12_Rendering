#include "MeshComponent.h"

MeshProxy::MeshProxy(MeshComponent* InComponent)
    : Parent(InComponent)
{

}

MeshComponent::MeshComponent(Scene* InScene, Component* InParent /*= nullptr*/)
    : Parent(InScene, InParent)
{

}

MeshComponent::~MeshComponent()
{

}

PrimitiveProxy* MeshComponent::CreateProxy()
{
    MeshProxy* proxy = new MeshProxy(this);
    proxy->PrimitiveData.MaterialShaderProxy = &MeshMaterial->GetProxy();

    Proxy = proxy;

    return Proxy;
}

void MeshComponent::SetMaterialComponent(std::shared_ptr<MaterialComponent> InMaterialComponent)
{
    MeshMaterial = InMaterialComponent;
}
