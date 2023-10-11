#include "PrimitiveComponent.h"
#include "RenderInterface.h"
#include "Scene.h"

MeshLoader* PrimitiveBuilder::Loader = nullptr;

void PrimitiveBuilder::Build(PrimitiveProxy* InProxy)
{
    if (!InProxy)
    {
		return;
    }

	VertexStream vertices;
	GetLoader().GetVertices(vertices);

	IndexStream indices;
    GetLoader().GetIndices(indices);

    InProxy->PrimitiveData.Vertices = std::move(vertices);
	InProxy->PrimitiveData.Stride = sizeof(vertices[0]);
    InProxy->PrimitiveData.Indices = std::move(indices);
}

void PrimitiveBuilder::LoadStaticMesh(const std::wstring& InPath)
{
	GetLoader().LoadObj(InPath.c_str());
}

MeshLoader& PrimitiveBuilder::GetLoader()
{
	if (!Loader)
	{
		Loader = MeshLoader::Create();
	}
	return *Loader;
}

PrimitiveProxy::PrimitiveProxy(PrimitiveComponent* InComponent)
	: OwnerComponent(InComponent)
{

}

std::unique_ptr<PrimitiveBuilder> PrimitiveComponent::Builder;

PrimitiveComponent::PrimitiveComponent(Scene* InScene, Component* InParent)
	: Parent(InScene, InParent)
{

}

PrimitiveComponent::~PrimitiveComponent()
{
	SafeDelete(Proxy);
}

void PrimitiveComponent::SetMeshModel(const std::wstring& InPath)
{
	if (!Builder)
	{
		Builder = std::make_unique<PrimitiveBuilder>();
	}

	CreateResource();

	Builder->LoadStaticMesh(InPath);
	Builder->Build(Proxy);
}

PrimitiveProxy* PrimitiveComponent::CreateProxy()
{
	PrimitiveProxy* proxy = new PrimitiveProxy(this);
	assert(proxy);

	Proxy = proxy;

	return proxy;
}

void PrimitiveComponent::CreateResource()
{
	GetScene()->AddPrimitive(this);
}

void PrimitiveComponent::Update()
{

}