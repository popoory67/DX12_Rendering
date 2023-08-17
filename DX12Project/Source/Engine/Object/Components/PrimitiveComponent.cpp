#include "PrimitiveComponent.h"
#include "RenderInterface.h"
#include "Scene.h"

void PrimitiveBuilder::Build(VertexStream&& InVertexStream, PrimitiveProxy* InProxy)
{
    if (!InProxy)
    {
		return;
    }

	InProxy->PrimitiveData = std::move(InVertexStream);
}

PrimitiveProxy::PrimitiveProxy(PrimitiveComponent* InComponent)
	: OwnerComponent(InComponent)
{

}

PrimitiveBuilder* PrimitiveComponent::Builder = nullptr;

PrimitiveComponent::PrimitiveComponent(Scene* InScene, Component* InParent)
	: Parent(InScene, InParent)
{

}

PrimitiveComponent::~PrimitiveComponent()
{
}

void PrimitiveComponent::SetMeshModel(const std::string& InPath)
{
	if (!Builder)
	{
		Builder = new PrimitiveBuilder();
	}

	CreateResource();

	// TODO
	// Mesh data loader has to be developed.
	VertexStream triangleVertices =
    {
        { { 0.0f, 0.25f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { 0.25f, -0.25f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { -0.25f, -0.25f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
    };

	Builder->Build(std::move(triangleVertices), Proxy);
}

PrimitiveProxy* PrimitiveComponent::CreateProxy()
{
	PrimitiveProxy* proxy = new PrimitiveProxy(this);
	assert(proxy);

	return proxy;
}

void PrimitiveComponent::CreateResource()
{
	GetScene()->AddPrimitive(this);
}

void PrimitiveComponent::Update()
{

}